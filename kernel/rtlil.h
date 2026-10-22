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
	गणना State : अचिन्हित वर्ण {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	गणना SyncType : अचिन्हित वर्ण {
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
	गणना ConstFlags : अचिन्हित वर्ण {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	गणना SelectPartials : अचिन्हित वर्ण {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	गणना SelectBoxes : अचिन्हित वर्ण {
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

	गणना PortDir : अचिन्हित वर्ण  {
		PD_UNKNOWN = 0,
		PD_आगत = 1,
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
			अचर dict<IdString, RTLIL::Const> &src,
			अचर RTLIL::Design *src_design, RTLIL::Design *dst_design);

	बाह्य dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	अचर pool<IdString> &builtin_ff_cell_types();

	स्थिर अन्तःस्थ std::string escape_id(अचर std::string &str) {
		यदि (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			फर्काऊ "\\" + str;
		फर्काऊ str;
	}

	स्थिर अन्तःस्थ std::string unescape_id(अचर std::string &str) {
		यदि (str.size() < 2)
			फर्काऊ str;
		यदि (str[0] != '\\')
			फर्काऊ str;
		यदि (str[1] == '$' || str[1] == '\\')
			फर्काऊ str;
		यदि (str[1] >= '0' && str[1] <= '9')
			फर्काऊ str;
		फर्काऊ str.substr(1);
	}

	ढाँचा <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	ढाँचा <typename T> संरचना sort_by_name_str {
		bool चालक()(T *a, T *b) अचर {
			फर्काऊ a->name.lt_by_name(b->name);
		}
	};

	संरचना sort_by_id_str {
		अचर TwinePool& pool;
		explicit sort_by_id_str(अचर TwinePool& pool) : pool(pool) {}
		bool चालक()(RTLIL::IdString a, RTLIL::IdString b) अचर {
			फर्काऊ pool.compare_by_name(a, b) < 0;
		}
	};

	स्थिर अन्तःस्थ std::string encode_filename(अचर std::string &filename)
	{
		std::stringstream val;
		यदि (!std::any_of(filename.begin(), filename.end(), [](वर्ण c) {
			फर्काऊ static_cast<अचिन्हित वर्ण>(c) < 33 || static_cast<अचिन्हित वर्ण>(c) > 126;
		})) फर्काऊ filename;
		लागि (अचिन्हित वर्ण अचर c : filename) {
			यदि (c < 33 || c > 126)
				val << stringf("$%02x", c);
			अन्यथा
				val << c;
		}
		फर्काऊ val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_and         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_or          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_xor         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_xnor        (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_reduce_and  (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_or   (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_xor  (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_xnor (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_bool (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_logic_not   (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_logic_and   (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_logic_or    (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_shl         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_shr         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_sshl        (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_sshr        (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_shift       (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_shiftx      (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_lt          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_le          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_eq          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_ne          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_eqx         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_nex         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_ge          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_gt          (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_add         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_sub         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_mul         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_div         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_divfloor    (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_modfloor    (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_mod         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_pow         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_pos         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_buf         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_neg         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_mux         (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, अचर RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, अचर RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2);
	RTLIL::Const const_demux       (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (अचर RTLIL::Const &arg1, अचर RTLIL::Const &arg2, अचर RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	ढाँचा<typename T, typename साँचो_2 = IdString>
	संरचना ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using मान_type = T;
		using difference_type = सूचक_भेद_प्रकार;
		using pointer = T*;
		using reference = T&;
		typename dict<साँचो_2, T>::iterator it;
		dict<साँचो_2, T> *list_p;
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

		ObjIterator(अचर RTLIL::ObjIterator<T, साँचो_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			यदि (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &चालक=(अचर RTLIL::ObjIterator<T, साँचो_2> &other) {
			यदि (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			यदि (refcount_p)
				(*refcount_p)++;
			फर्काऊ *this;
		}

		~ObjIterator() {
			यदि (refcount_p)
				(*refcount_p)--;
		}

		अन्तःस्थ T चालक*() अचर {
			log_assert(list_p != nullptr);
			फर्काऊ it->second;
		}

		अन्तःस्थ bool चालक!=(अचर RTLIL::ObjIterator<T, साँचो_2> &other) अचर {
			यदि (list_p == nullptr || other.list_p == nullptr)
				फर्काऊ list_p != other.list_p;
			फर्काऊ it != other.it;
		}


		अन्तःस्थ bool चालक==(अचर RTLIL::ObjIterator<T, साँचो_2> &other) अचर {
			फर्काऊ !(*this != other);
		}

		अन्तःस्थ ObjIterator<T, साँचो_2>& चालक++() {
			log_assert(list_p != nullptr);
			यदि (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			फर्काऊ *this;
		}

		अन्तःस्थ ObjIterator<T, साँचो_2>& चालक+=(पूर्णाङ्क amt) {
			log_assert(list_p != nullptr);
			it += amt;
			यदि (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			फर्काऊ *this;
		}

		अन्तःस्थ ObjIterator<T, साँचो_2> चालक+(पूर्णाङ्क amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, साँचो_2> new_obj(*this);
			new_obj.it += amt;
			यदि (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			फर्काऊ new_obj;
		}

		अन्तःस्थ अचर ObjIterator<T, साँचो_2> चालक++(पूर्णाङ्क) {
			ObjIterator<T, साँचो_2> result(*this);
			++(*this);
			फर्काऊ result;
		}
	};

	ढाँचा<typename T, typename साँचो_2 = IdString>
	संरचना ObjRange
	{
		dict<साँचो_2, T> *list_p;
		पूर्णाङ्क *refcount_p;

		ObjRange(decltype(list_p) list_p, पूर्णाङ्क *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, साँचो_2> begin() { फर्काऊ RTLIL::ObjIterator<T, साँचो_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, साँचो_2> end() { फर्काऊ RTLIL::ObjIterator<T, साँचो_2>(); }

		माप_प्रकार size() अचर {
			फर्काऊ list_p->size();
		}

		चालक pool<T>() अचर {
			pool<T> result;
			लागि (स्वतः &it : *list_p)
				result.घुसाउ(it.second);
			फर्काऊ result;
		}

		चालक std::vector<T>() अचर {
			std::vector<T> result;
			result.reserve(list_p->size());
			लागि (स्वतः &it : *list_p)
				result.थप_back(it.second);
			फर्काऊ result;
		}

		pool<T> to_pool() अचर { फर्काऊ *this; }
		std::vector<T> to_vector() अचर { फर्काऊ *this; }
	};
};

संरचना RTLIL::Const
{
	लघु पूर्णाङ्क flags;
गुप्त:
	मित्र class KernelRtlilTest;
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
	bool is_bits() अचर { फर्काऊ tag == backing_tag::bits; }
	bool is_str() अचर { फर्काऊ tag == backing_tag::string; }

	bitvectype* get_if_bits() { फर्काऊ is_bits() ? &bits_ : रिक्त; }
	std::string* get_if_str() { फर्काऊ is_str() ? &str_ : रिक्त; }
	अचर bitvectype* get_if_bits() अचर { फर्काऊ is_bits() ? &bits_ : रिक्त; }
	अचर std::string* get_if_str() अचर { फर्काऊ is_str() ? &str_ : रिक्त; }

	bitvectype& get_bits();
	std::string& get_str();
	अचर bitvectype& get_bits() अचर;
	अचर std::string& get_str() अचर;
	std::vector<RTLIL::State>& bits_internal();
	शून्य bitvectorize_internal();

सार्वजनिक:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(दीर्घ दीर्घ val); // default width is 32
	Const(दीर्घ दीर्घ val, पूर्णाङ्क width);
	Const(RTLIL::State bit, पूर्णाङ्क width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(अचर std::vector<bool> &bits);
	Const(अचर RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &चालक =(अचर RTLIL::Const &other);
	~Const();

	संरचना Builder
	{
		Builder() {}
		Builder(पूर्णाङ्क expected_width) { bits.reserve(expected_width); }
		शून्य थप_back(RTLIL::State b) { bits.थप_back(b); }
		पूर्णाङ्क size() अचर { फर्काऊ static_cast<पूर्णाङ्क>(bits.size()); }
		Const build() { फर्काऊ Const(std::move(bits)); }
	गुप्त:
		std::vector<RTLIL::State> bits;
	};

	bool चालक <(अचर RTLIL::Const &other) अचर;
	bool चालक ==(अचर RTLIL::Const &other) अचर;
	bool चालक !=(अचर RTLIL::Const &other) अचर;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { फर्काऊ bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	शून्य bitvectorize() अचर { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() अचर;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णाङ्क as_int(bool is_signed = false) अचर;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) अचर;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णाङ्क> try_as_int(bool is_signed = false) अचर;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	पूर्णाङ्क as_int_saturating(bool is_signed = false) अचर;

	शून्य tag_bare_integer_const(अचर std::string &मान);

	std::string as_string(अचर वर्ण* any = "-") अचर;
	स्थिर Const from_string(अचर std::string &str);
	std::vector<RTLIL::State> to_bits() अचर;

	std::string decode_string() अचर;
	पूर्णाङ्क size() अचर;
	bool empty() अचर;

	शून्य append(अचर RTLIL::Const &other);
	शून्य set(पूर्णाङ्क i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	शून्य resize(पूर्णाङ्क size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	गुप्त:
		अचर Const* parent;
		माप_प्रकार idx;

	सार्वजनिक:
		using iterator_category = std::bidirectional_iterator_tag;
		using मान_type = State;
		using difference_type = std::सूचक_भेद_प्रकार;
		using pointer = अचर State*;
		using reference = अचर State&;

		const_iterator(अचर Const& c, माप_प्रकार i) : parent(&c), idx(i) {}

		State चालक*() अचर;

		const_iterator& चालक++() { ++idx; फर्काऊ *this; }
		const_iterator& चालक--() { --idx; फर्काऊ *this; }
		const_iterator चालक++(पूर्णाङ्क) { const_iterator result(*this); ++idx; फर्काऊ result; }
		const_iterator चालक--(पूर्णाङ्क) { const_iterator result(*this); --idx; फर्काऊ result; }
		const_iterator& चालक+=(पूर्णाङ्क i) { idx += i; फर्काऊ *this; }

		const_iterator चालक+(पूर्णाङ्क add) {
			फर्काऊ const_iterator(*parent, idx + add);
		}
		const_iterator चालक-(पूर्णाङ्क sub) {
			फर्काऊ const_iterator(*parent, idx - sub);
		}
		पूर्णाङ्क चालक-(अचर const_iterator& other) {
			फर्काऊ idx - other.idx;
		}

		bool चालक==(अचर const_iterator& other) अचर {
			फर्काऊ idx == other.idx;
		}

		bool चालक!=(अचर const_iterator& other) अचर {
			फर्काऊ !(*this == other);
		}
	};

	class iterator {
	गुप्त:
		Const* parent;
		माप_प्रकार idx;

	सार्वजनिक:
		class proxy {
		गुप्त:
			Const* parent;
			माप_प्रकार idx;
		सार्वजनिक:
			proxy(Const* parent, माप_प्रकार idx) : parent(parent), idx(idx) {}
			चालक State() अचर { फर्काऊ (*parent)[idx]; }
			proxy& चालक=(State s) { parent->set(idx, s); फर्काऊ *this; }
			proxy& चालक=(अचर proxy& other) { parent->set(idx, (*other.parent)[other.idx]); फर्काऊ *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using मान_type = State;
		using difference_type = std::सूचक_भेद_प्रकार;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, माप_प्रकार i) : parent(&c), idx(i) {}

		proxy चालक*() अचर { फर्काऊ proxy(parent, idx); }
		iterator& चालक++() { ++idx; फर्काऊ *this; }
		iterator& चालक--() { --idx; फर्काऊ *this; }
		iterator चालक++(पूर्णाङ्क) { iterator result(*this); ++idx; फर्काऊ result; }
		iterator चालक--(पूर्णाङ्क) { iterator result(*this); --idx; फर्काऊ result; }
		iterator& चालक+=(पूर्णाङ्क i) { idx += i; फर्काऊ *this; }

		iterator चालक+(पूर्णाङ्क add) {
			फर्काऊ iterator(*parent, idx + add);
		}
		iterator चालक-(पूर्णाङ्क sub) {
			फर्काऊ iterator(*parent, idx - sub);
		}
		पूर्णाङ्क चालक-(अचर iterator& other) {
			फर्काऊ idx - other.idx;
		}

		bool चालक==(अचर iterator& other) अचर {
			फर्काऊ idx == other.idx;
		}

		bool चालक!=(अचर iterator& other) अचर {
			फर्काऊ !(*this == other);
		}
	};

	const_iterator begin() अचर {
		फर्काऊ const_iterator(*this, 0);
	}
	const_iterator end() अचर {
		फर्काऊ const_iterator(*this, size());
	}
	iterator begin() {
		फर्काऊ iterator(*this, 0);
	}
	iterator end() {
		फर्काऊ iterator(*this, size());
	}
	State back() अचर {
		फर्काऊ *(end() - 1);
	}
	State front() अचर {
		फर्काऊ *begin();
	}
	State at(माप_प्रकार i) अचर {
		फर्काऊ *const_iterator(*this, i);
	}
	State चालक[](माप_प्रकार i) अचर {
		फर्काऊ *const_iterator(*this, i);
	}

	bool is_fully_zero() अचर;
	bool is_fully_ones() अचर;
	bool is_fully_def() अचर;
	bool is_fully_undef() अचर;
	bool is_fully_undef_x_only() अचर;
	bool is_onehot(पूर्णाङ्क *pos = nullptr) अचर;

	RTLIL::Const extract(पूर्णाङ्क offset, पूर्णाङ्क len = 1, RTLIL::State padding = RTLIL::State::S0) अचर;

	// find the MSB without redundant leading bits
	पूर्णाङ्क get_min_size(bool is_signed) अचर;

	// compress representation to the minimum required bits
	शून्य compress(bool is_signed = false);

	std::optional<पूर्णाङ्क> as_int_compress(bool is_signed) अचर;

	शून्य extu(पूर्णाङ्क width) {
		resize(width, RTLIL::State::S0);
	}

	शून्य exts(पूर्णाङ्क width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;
};

संरचना RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) अचर;

	शून्य set_bool_attribute(RTLIL::IdString id, bool मान=true);
	bool get_bool_attribute(RTLIL::IdString id) अचर;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) अचर {
		फर्काऊ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य set_string_attribute(RTLIL::IdString id, string मान);
	string get_string_attribute(RTLIL::IdString id) अचर;

	शून्य set_strpool_attribute(RTLIL::IdString id, अचर pool<string> &data);
	शून्य add_strpool_attribute(RTLIL::IdString id, अचर pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) अचर;

	शून्य set_src_attribute(अचर std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() अचर {
		फर्काऊ get_string_attribute(ID::src);
	}

	शून्य set_hdlname_attribute(अचर vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() अचर;

	शून्य set_intvec_attribute(RTLIL::IdString id, अचर vector<पूर्णाङ्क> &data);
	vector<पूर्णाङ्क> get_intvec_attribute(RTLIL::IdString id) अचर;
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
	SigChunk(अचर RTLIL::Const &मान) : wire(nullptr), data(मान.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&मान) : wire(nullptr), data(मान.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(अचर std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(पूर्णाङ्क val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(पूर्णाङ्क val, पूर्णाङ्क width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, पूर्णाङ्क width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(अचर RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(पूर्णाङ्क offset, पूर्णाङ्क length) अचर;
	RTLIL::SigBit चालक[](पूर्णाङ्क offset) अचर;
	अन्तःस्थ पूर्णाङ्क size() अचर { फर्काऊ width; }
	अन्तःस्थ bool is_wire() अचर { फर्काऊ wire != रिक्त; }

	bool चालक <(अचर RTLIL::SigChunk &other) अचर;
	bool चालक ==(अचर RTLIL::SigChunk &other) अचर;
	bool चालक !=(अचर RTLIL::SigChunk &other) अचर;
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
	SigBit(अचर RTLIL::SigChunk &chunk);
	SigBit(अचर RTLIL::SigChunk &chunk, पूर्णाङ्क index);
	SigBit(अचर RTLIL::SigSpec &sig);
	SigBit(अचर RTLIL::SigBit &sigbit) = स्वभाव;
	RTLIL::SigBit &चालक =(अचर RTLIL::SigBit &other) = स्वभाव;

	अन्तःस्थ bool is_wire() अचर { फर्काऊ wire != रिक्त; }

	bool चालक <(अचर RTLIL::SigBit &other) अचर;
	bool चालक ==(अचर RTLIL::SigBit &other) अचर;
	bool चालक !=(अचर RTLIL::SigBit &other) अचर;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर;
	[[nodiscard]] Hasher hash_शीर्ष() अचर;
};

namespace hashlib {
	ढाँचा <>
	संरचना hash_ops<RTLIL::SigBit> {
		स्थिर अन्तःस्थ bool cmp(अचर RTLIL::SigBit &a, अचर RTLIL::SigBit &b) {
			फर्काऊ a == b;
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(अचर RTLIL::SigBit sb) {
			फर्काऊ sb.hash_शीर्ष();
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर RTLIL::SigBit sb, Hasher h) {
			फर्काऊ sb.hash_into(h);
		}
	};
};

संरचना RTLIL::SigSpecIterator
{
	प्रकारनिर्देश std::आगत_iterator_tag iterator_category;
	प्रकारनिर्देश RTLIL::SigBit मान_type;
	प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
	प्रकारनिर्देश RTLIL::SigBit* pointer;
	प्रकारनिर्देश RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	पूर्णाङ्क index;

	अन्तःस्थ RTLIL::SigBit &चालक*() अचर;
	अन्तःस्थ bool चालक!=(अचर RTLIL::SigSpecIterator &other) अचर { फर्काऊ index != other.index; }
	अन्तःस्थ bool चालक==(अचर RTLIL::SigSpecIterator &other) अचर { फर्काऊ index == other.index; }
	अन्तःस्थ शून्य चालक++() { index++; }
};

संरचना RTLIL::SigSpecConstIterator
{
	प्रकारनिर्देश std::आगत_iterator_tag iterator_category;
	प्रकारनिर्देश RTLIL::SigBit मान_type;
	प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
	प्रकारनिर्देश RTLIL::SigBit* pointer;
	प्रकारनिर्देश RTLIL::SigBit& reference;

	अचर RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	पूर्णाङ्क index;

	अन्तःस्थ अचर RTLIL::SigBit &चालक*();
	अन्तःस्थ bool चालक!=(अचर RTLIL::SigSpecConstIterator &other) अचर { फर्काऊ index != other.index; }
	अन्तःस्थ bool चालक==(अचर RTLIL::SigSpecIterator &other) अचर { फर्काऊ index == other.index; }
	अन्तःस्थ शून्य चालक++() { index++; }
};

संरचना RTLIL::SigSpec
{
गुप्त:
	मित्र class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	गणना Representation : वर्ण {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	संरचना AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(अचर AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &चालक=(अचर AtomicHash &rhs) { store(rhs.load()); फर्काऊ *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> पठ() अचर {
			Hasher::hash_t मान = load();
			यदि (मान == 0)
				फर्काऊ std::nullopt;
			फर्काऊ मान;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		शून्य set(Hasher::hash_t मान) अचर {
			log_assert(मान != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(मान, std::memory_order_relaxed);
			log_assert(old == 0 || old == मान);
		}
		शून्य clear() { store(0); }
	गुप्त:
		पूर्णाङ्क load() अचर { फर्काऊ atomic_.load(std::memory_order_relaxed); }
		शून्य store(Hasher::hash_t मान) अचर {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(मान, std::memory_order_relaxed);
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

	Hasher::hash_t updhash() अचर;
	शून्य destroy() {
		यदि (rep_ == CHUNK)
			chunk_.~SigChunk();
		अन्यथा
			bits_.~vector();
	}
	मित्र संरचना Chunks;

सार्वजनिक:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(अचर SigSpec &मान) : rep_(मान.rep_), hash_(मान.hash_) {
		यदि (मान.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(मान.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(मान.bits_);
	}
	SigSpec(SigSpec &&मान) : rep_(मान.rep_), hash_(मान.hash_) {
		यदि (मान.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(मान.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(मान.bits_));
	}
	SigSpec(अचर RTLIL::Const &मान);
	SigSpec(RTLIL::Const &&मान);
	SigSpec(अचर RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width = 1);
	SigSpec(अचर std::string &str);
	SigSpec(पूर्णाङ्क val, पूर्णाङ्क width = 32);
	SigSpec(RTLIL::State bit, पूर्णाङ्क width = 1);
	SigSpec(अचर RTLIL::SigBit &bit, पूर्णाङ्क width = 1);
	SigSpec(अचर std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(अचर std::vector<RTLIL::SigBit> &bits);
	SigSpec(अचर pool<RTLIL::SigBit> &bits);
	SigSpec(अचर std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &चालक=(अचर SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		यदि (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		फर्काऊ *this;
	}
	SigSpec &चालक=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		यदि (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		फर्काऊ *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	संरचना Chunks {
		Chunks(अचर SigSpec &spec) : spec(spec) {}
		संरचना const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using मान_type = अचर SigChunk &;
			using difference_type = std::सूचक_भेद_प्रकार;
			using pointer = अचर SigChunk *;
			using reference = अचर SigChunk &;

			अचर SigSpec &spec;
			पूर्णाङ्क bit_index;
			SigChunk chunk;

			const_iterator(अचर SigSpec &spec) : spec(spec) {
				bit_index = 0;
				यदि (spec.rep_ == BITS)
					next_chunk_bits();
			}
			गणना End { END };
			const_iterator(अचर SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			शून्य next_chunk_bits();

			अचर SigChunk &चालक*() {
				यदि (spec.rep_ == CHUNK)
					फर्काऊ spec.chunk_;
				फर्काऊ chunk;
			};
			अचर SigChunk *चालक->() { फर्काऊ &**this; }
			const_iterator &चालक++() {
				bit_index += (**this).width;
				यदि (spec.rep_ == BITS)
					next_chunk_bits();
				फर्काऊ *this;
			}
			bool चालक==(अचर const_iterator &rhs) अचर { फर्काऊ bit_index == rhs.bit_index; }
			bool चालक!=(अचर const_iterator &rhs) अचर { फर्काऊ !(*this == rhs); }
		};
		const_iterator begin() अचर { फर्काऊ const_iterator(spec); }
		const_iterator end() अचर {
			const_iterator it(spec, const_iterator::END);
			फर्काऊ it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			फर्काऊ chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			फर्काऊ chunk_vector.rend();
		}
		पूर्णाङ्क size() {
			ensure_chunk_vector();
			फर्काऊ chunk_vector.size();
		}
		पूर्णाङ्क size() अचर {
			फर्काऊ std::distance(begin(), end());
		}
		अचर SigChunk &at(पूर्णाङ्क index) {
			ensure_chunk_vector();
			फर्काऊ chunk_vector.at(index);
		}
		चालक अचर std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			फर्काऊ chunk_vector;
		}
	गुप्त:
		शून्य ensure_chunk_vector() {
			यदि (spec.size() > 0 && chunk_vector.empty()) {
				लागि (अचर RTLIL::SigChunk &c : *this)
					chunk_vector.थप_back(c);
			}
		}
		अचर SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	मित्र संरचना Chunks::const_iterator;

	अन्तःस्थ Chunks chunks() अचर { फर्काऊ {*this}; }
	अन्तःस्थ अचर SigSpec &bits() अचर { फर्काऊ *this; }

	अन्तःस्थ पूर्णाङ्क size() अचर { फर्काऊ rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	अन्तःस्थ bool empty() अचर { फर्काऊ size() == 0; };

	अन्तःस्थ RTLIL::SigBit &चालक[](पूर्णाङ्क index) { inline_unpack(); hash_.clear(); फर्काऊ bits_.at(index); }
	अन्तःस्थ RTLIL::SigBit चालक[](पूर्णाङ्क index) अचर {
		यदि (rep_ == CHUNK) {
			यदि (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			यदि (chunk_.wire)
				फर्काऊ RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			फर्काऊ RTLIL::SigBit(chunk_.data[index]);
		}
		फर्काऊ bits_.at(index);
	}

	अन्तःस्थ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; फर्काऊ it; }
	अन्तःस्थ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); फर्काऊ it; }

	अन्तःस्थ RTLIL::SigSpecConstIterator begin() अचर { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; फर्काऊ it; }
	अन्तःस्थ RTLIL::SigSpecConstIterator end() अचर { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); फर्काऊ it; }

	शून्य sort();
	शून्य sort_and_unify();

	शून्य replace(अचर RTLIL::SigSpec &pattern, अचर RTLIL::SigSpec &with);
	शून्य replace(अचर RTLIL::SigSpec &pattern, अचर RTLIL::SigSpec &with, RTLIL::SigSpec *other) अचर;

	शून्य replace(अचर dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	शून्य replace(अचर dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) अचर;

	शून्य replace(अचर std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	शून्य replace(अचर std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) अचर;

	शून्य replace(पूर्णाङ्क offset, अचर RTLIL::SigSpec &with);

	शून्य हटाओ(अचर RTLIL::SigSpec &pattern);
	शून्य हटाओ(अचर RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) अचर;
	शून्य remove2(अचर RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	शून्य हटाओ(अचर pool<RTLIL::SigBit> &pattern);
	शून्य हटाओ(अचर pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) अचर;
	शून्य remove2(अचर pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	शून्य remove2(अचर std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	शून्य remove2(अचर pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	शून्य हटाओ(पूर्णाङ्क offset, पूर्णाङ्क length = 1);
	शून्य remove_const();

	RTLIL::SigSpec extract(अचर RTLIL::SigSpec &pattern, अचर RTLIL::SigSpec *other = रिक्त) अचर;
	RTLIL::SigSpec extract(अचर pool<RTLIL::SigBit> &pattern, अचर RTLIL::SigSpec *other = रिक्त) अचर;
	RTLIL::SigSpec extract(पूर्णाङ्क offset, पूर्णाङ्क length = 1) अचर;
	RTLIL::SigSpec extract_end(पूर्णाङ्क offset) अचर { फर्काऊ extract(offset, size() - offset); }

	शून्य rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() अचर { log_assert(size()); फर्काऊ (*this)[0]; };
	RTLIL::SigBit msb() अचर { log_assert(size()); फर्काऊ (*this)[size() - 1]; };
	RTLIL::SigBit front() अचर { फर्काऊ (*this)[0]; }
	RTLIL::SigBit back() अचर { फर्काऊ (*this)[size() - 1]; }

	शून्य append(अचर RTLIL::SigSpec &सङ्केत);
	अन्तःस्थ शून्य append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	अन्तःस्थ शून्य append(अचर RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	अन्तःस्थ शून्य append(अचर RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	शून्य append(अचर RTLIL::SigBit &bit);
	अन्तःस्थ शून्य append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	अन्तःस्थ शून्य append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	शून्य extend_u0(पूर्णाङ्क width, bool is_signed = false);

	RTLIL::SigSpec repeat(पूर्णाङ्क अङ्क) अचर;

	शून्य reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool चालक <(अचर RTLIL::SigSpec &other) अचर;
	bool चालक ==(अचर RTLIL::SigSpec &other) अचर;
	अन्तःस्थ bool चालक !=(अचर RTLIL::SigSpec &other) अचर { फर्काऊ !(*this == other); }

	bool is_wire() अचर;
	bool is_chunk() अचर;
	अन्तःस्थ bool is_bit() अचर { फर्काऊ size() == 1; }

	bool known_driver() अचर;

	bool is_fully_const() अचर;
	bool is_fully_zero() अचर;
	bool is_fully_ones() अचर;
	bool is_fully_def() अचर;
	bool is_fully_undef() अचर;
	bool has_const() अचर;
	bool has_const(State state) अचर;
	bool has_marked_bits() अचर;
	bool is_onehot(पूर्णाङ्क *pos = nullptr) अचर;

	bool as_bool() अचर;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णाङ्क as_int(bool is_signed = false) अचर;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) अचर;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णाङ्क> try_as_int(bool is_signed = false) अचर;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	पूर्णाङ्क as_int_saturating(bool is_signed = false) अचर;

	std::string as_string() अचर;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() अचर;
	RTLIL::Const as_const() अचर;
	RTLIL::Wire *as_wire() अचर;
	RTLIL::SigChunk as_chunk() अचर;
	RTLIL::SigBit as_bit() अचर;

	bool match(अचर वर्ण* pattern) अचर;

	std::set<RTLIL::SigBit> to_sigbit_set() अचर;
	pool<RTLIL::SigBit> to_sigbit_pool() अचर;
	std::vector<RTLIL::SigBit> to_sigbit_vector() अचर;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(अचर RTLIL::SigSpec &other) अचर;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(अचर RTLIL::SigSpec &other) अचर;

	स्थिर bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	स्थिर bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	स्थिर bool parse_rhs(अचर RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	चालक std::vector<RTLIL::SigChunk>() अचर;
	चालक std::vector<RTLIL::SigBit>() अचर { फर्काऊ to_sigbit_vector(); }
	अचर RTLIL::SigBit &at(पूर्णाङ्क offset, अचर RTLIL::SigBit &defval) { फर्काऊ offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(पूर्णाङ्क offset) { फर्काऊ (*this)[offset]; }
	RTLIL::SigBit at(पूर्णाङ्क offset) अचर { फर्काऊ (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) अचर {
		Hasher::hash_t val;
		यदि (std::optional<Hasher::hash_t> current = hash_.पठ())
			val = *current;
		अन्यथा
			val = updhash();
		h.eat(val);
		फर्काऊ h;
	}

#ifndef न_संशोधन
	शून्य check(अचर Module *mod = nullptr) अचर;
#अन्यथा
	शून्य check(अचर Module *mod = nullptr) अचर { (शून्य)mod; }
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
	bool boxed_module(RTLIL::IdString mod_name) अचर;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) अचर;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) अचर;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) अचर;

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
	bool selects_all() अचर {
		फर्काऊ full_selection || complete_selection;
	}

	// add whole module to this selection
	ढाँचा<typename T1> शून्य select(T1 *module) {
		यदि (!selects_all() && selected_modules.गन्ती(module->name) == 0) {
			IdString name = module->name;
			selected_modules.घुसाउ(name);
			selected_members.erase(name);
			यदि (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	ढाँचा<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	ढाँचा<typename T1> शून्य select(T1 *module, RTLIL::IdString member) {
		यदि (!selects_all() && selected_modules.गन्ती(module->name.ref()) == 0) {
			selected_members[module->name.ref()].घुसाउ(member);
			यदि (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() अचर {
		फर्काऊ !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	शून्य clear();

	// create a new selection which is empty
	स्थिर Selection EmptySelection(RTLIL::Design *design = nullptr) { फर्काऊ Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	स्थिर Selection FullSelection(RTLIL::Design *design = nullptr) { फर्काऊ Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	स्थिर Selection CompleteSelection(RTLIL::Design *design = nullptr) { फर्काऊ Selection(true, true, design); };
};

संरचना RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

	Monitor() {
		स्थिर अचिन्हित पूर्णाङ्क hashidx_गन्ती = 123456789;
		hashidx_गन्ती = mkhash_xorshift(hashidx_गन्ती);
		hashidx_ = hashidx_गन्ती;
	}

	आभास ~Monitor() { }
	आभास शून्य notify_module_add(RTLIL::Module*) { }
	आभास शून्य notify_module_del(RTLIL::Module*) { }
	आभास शून्य notify_connect(RTLIL::Cell*, RTLIL::IdString, अचर RTLIL::SigSpec&, अचर RTLIL::SigSpec&) { }
	आभास शून्य notify_connect(RTLIL::Module*, अचर RTLIL::SigSig&) { }
	आभास शून्य notify_connect(RTLIL::Module*, अचर std::vector<RTLIL::SigSig>&) { }
	आभास शून्य notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
संरचना define_map_t;

ढाँचा<typename N>
अन्तःस्थ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, अचर वर्ण*> || std::is_same_v<std::decay_t<N>, वर्ण*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, पूर्णाङ्क> = 0

ढाँचा<typename N>
अन्तःस्थ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, अचर वर्ण *> || std::is_same_v<std::decay_t<N>, वर्ण *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, पूर्णाङ्क> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	ढाँचा<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(स्वतः) _func(N name, Rest&&... rest) \
		{ फर्काऊ _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	ढाँचा<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(स्वतः) _func(T &&first, N name) \
		{ फर्काऊ _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

संरचना RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	शून्य bufNormalize(bool enable=true);

	पूर्णाङ्क refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

गुप्त:
	TwinePool twines_;

सार्वजनिक:
	TwinePool &twines() { फर्काऊ twines_; }
	अचर TwinePool &twines() अचर { फर्काऊ twines_; }

	शून्य absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	माप_प्रकार gc_twines();

	std::vector<std::unique_ptr<AST::Ast_गाँठो>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_थाक;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	अचर RTLIL::Module *module(RTLIL::IdString name) अचर;
	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ फर्काऊ module(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> अचर RTLIL::Module *module(N name) अचर
		{ फर्काऊ module(twines().find(std::string_view(name))); }
	RTLIL::Module *शीर्ष_module() अचर;

	bool has(RTLIL::IdString id) अचर {
		twines_.check_owned(id);
		फर्काऊ modules_.गन्ती(id) != 0;
	}
	ढाँचा<typename N, YS_NAME_STRING(N)> bool has(N name) अचर
		{ फर्काऊ has(twines().find(std::string_view(name))); }

	शून्य add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	शून्य हटाओ(RTLIL::Module *module);
	शून्य नाम(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(नाम, twines())

	शून्य scratchpad_unset(अचर std::string &varname);

	शून्य scratchpad_set_int(अचर std::string &varname, पूर्णाङ्क मान);
	शून्य scratchpad_set_bool(अचर std::string &varname, bool मान);
	शून्य scratchpad_set_string(अचर std::string &varname, std::string मान);

	पूर्णाङ्क scratchpad_get_int(अचर std::string &varname, पूर्णाङ्क default_मान_2 = 0) अचर;
	bool scratchpad_get_bool(अचर std::string &varname, bool default_मान_2 = false) अचर;
	std::string scratchpad_get_string(अचर std::string &varname, अचर std::string &default_मान_2 = std::string()) अचर;

	शून्य sort();
	शून्य sort_modules();
	शून्य check();
	शून्य optimize();

	शून्य clone_into(RTLIL::Design *dst) अचर;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) अचर;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) अचर;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) अचर;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) अचर;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) अचर;

	// push the given selection to the selection stack
	शून्य थप_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	शून्य थप_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	शून्य थप_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	शून्य थप_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	शून्य झिक_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		फर्काऊ selection_थाक.back();
	}

	// get the current selection
	अचर RTLIL::Selection &selection() अचर {
		फर्काऊ selection_थाक.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() अचर {
		फर्काऊ selection().full_selection;
	}

	// is the given module in the current selection
	ढाँचा<typename T1> bool selected(T1 *module) अचर {
		फर्काऊ selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	ढाँचा<typename T1, typename T2> bool selected(T1 *module, T2 *member) अचर {
		फर्काऊ selected_member(module->name, member->name);
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
	) अचर;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() अचर { फर्काऊ selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() अचर { फर्काऊ selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() अचर { फर्काऊ selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() अचर { फर्काऊ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() अचर { फर्काऊ selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) अचर { फर्काऊ selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() अचर { फर्काऊ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() अचर { फर्काऊ selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	स्थिर std::map<अचिन्हित पूर्णाङ्क, RTLIL::Design*> *get_all_designs(शून्य);

	std::string to_rtlil_str(bool only_selected = true) अचर;
};

संरचना RTLIL::Module : सार्वजनिक RTLIL::AttrObject
{
	मित्र संरचना RTLIL::Cell;
	मित्र संरचना RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

रक्षित:
	शून्य add(RTLIL::Wire *wire);
	शून्य add(RTLIL::Cell *cell);
	शून्य add(RTLIL::Process *process);

सार्वजनिक:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() अचर { फर्काऊ design->twines(); }

	पूर्णाङ्क refcount_wires_;
	पूर्णाङ्क refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_मानहरू;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	आभास ~Module();
	आभास RTLIL::IdString derive(RTLIL::Design *design, अचर dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	आभास RTLIL::IdString derive(RTLIL::Design *design, अचर dict<RTLIL::IdString, RTLIL::Const> &parameters, अचर dict<RTLIL::IdString, RTLIL::Module*> &interfaces, अचर dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	आभास माप_प्रकार गन्ती_id(RTLIL::IdString id);
	आभास शून्य expand_interfaces(RTLIL::Design *design, अचर dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	आभास bool reprocess_if_necessary(RTLIL::Design *design);

	आभास शून्य sort();
	आभास शून्य check();
	आभास शून्य optimize();
	आभास शून्य makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) अचर {
		फर्काऊ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य connect(अचर RTLIL::SigSig &conn);
	शून्य connect(अचर RTLIL::SigSpec &lhs, अचर RTLIL::SigSpec &rhs);
	शून्य new_connections(अचर std::vector<RTLIL::SigSig> &new_conn);
	अचर std::vector<RTLIL::SigSig> &connections() अचर;

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
	शून्य cloneInto(RTLIL::Module *new_mod) अचर;
	आभास RTLIL::Module *clone() अचर;
	आभास RTLIL::Module *clone(RTLIL::Design *dst) अचर;
	आभास RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) अचर;

	bool has_memories() अचर;
	bool has_processes() अचर;

	bool has_memories_warn() अचर;
	bool has_processes_warn() अचर;

	bool is_selected() अचर;
	bool is_selected_whole() अचर;

	std::vector<RTLIL::Wire*> selected_wires() अचर;
	std::vector<RTLIL::Cell*> selected_cells() अचर;
	std::vector<RTLIL::Memory*> selected_memories() अचर;
	std::vector<RTLIL::Process*> selected_processes() अचर;
	std::vector<RTLIL::SelectedMember> selected_members() अचर;

	ढाँचा<typename T> bool selected(T *member) अचर {
		फर्काऊ design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		यदि (design)
			design->twines().check_owned(id);
		स्वतः it = wires_.find(id);
		फर्काऊ it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		यदि (design)
			design->twines().check_owned(id);
		स्वतः it = cells_.find(id);
		फर्काऊ it == cells_.end() ? nullptr : it->second;
	}

	अचर RTLIL::Wire* wire(RTLIL::IdString id) अचर{
		यदि (design)
			design->twines().check_owned(id);
		स्वतः it = wires_.find(id);
		फर्काऊ it == wires_.end() ? nullptr : it->second;
	}
	अचर RTLIL::Cell* cell(RTLIL::IdString id) अचर {
		यदि (design)
			design->twines().check_owned(id);
		स्वतः it = cells_.find(id);
		फर्काऊ it == cells_.end() ? nullptr : it->second;
	}

	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ फर्काऊ wire(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ फर्काऊ cell(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> अचर RTLIL::Wire *wire(N name) अचर
		{ फर्काऊ wire(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> अचर RTLIL::Cell *cell(N name) अचर
		{ फर्काऊ cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { फर्काऊ RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	पूर्णाङ्क wires_size() अचर { फर्काऊ wires_.size(); }
	RTLIL::Wire* wire_at(पूर्णाङ्क index) अचर { फर्काऊ wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { फर्काऊ RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	पूर्णाङ्क cells_size() अचर { फर्काऊ cells_.size(); }
	RTLIL::Cell* cell_at(पूर्णाङ्क index) अचर { फर्काऊ cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	शून्य हटाओ(अचर pool<RTLIL::Wire*> &wires);
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
	RTLIL::IdString uniquify(RTLIL::IdString name, पूर्णाङ्क &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, पूर्णाङ्क width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, अचर RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, अचर RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	ढाँचा<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { फर्काऊ addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, अचर RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, अचर RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_c, अचर RTLIL::SigSpec &sig_x, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, bool is_signed = false, अचर std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, RTLIL::Const offset, अचर std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_y, RTLIL::Const lut, अचर std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_en, अचर std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_en, अचर std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_en, अचर std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_en, अचर std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_en, अचर std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr, अचर RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, अचर std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_d,   अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en,  अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_arst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, RTLIL::Const arst_मान, bool clk_polarity = true, bool arst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_arst,  अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, RTLIL::Const arst_मान, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_aload, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, अचर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_aload,  अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, अचर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_srst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, RTLIL::Const srst_मान, bool clk_polarity = true, bool srst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_srst,  अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, RTLIL::Const srst_मान, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_srst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, RTLIL::Const srst_मान, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, bool en_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_arst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, RTLIL::Const arst_मान, bool en_polarity = true, bool arst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, अचर RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_s, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_s, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर RTLIL::SigBit &sig_d, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर RTLIL::SigBit &sig_d, अचर RTLIL::SigBit &sig_y, अचर std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr,
			अचर RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, अचर std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, अचर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_arst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			bool arst_मान = false, bool clk_polarity = true, bool arst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_arst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			bool arst_मान = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_aload, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			अचर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_aload, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			अचर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_srst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			bool srst_मान = false, bool clk_polarity = true, bool srst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_srst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			bool srst_मान = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_clk, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_srst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			bool srst_मान = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, bool en_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_arst, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q,
			bool arst_मान = false, bool en_polarity = true, bool arst_polarity = true, अचर std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_en, अचर RTLIL::SigSpec &sig_set, अचर RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, अचर RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, अचर std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, अचर RTLIL::SigSpec &sig_d, अचर RTLIL::SigSpec &sig_q, अचर std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, bool is_signed = false, अचर std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_s, अचर std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_s, अचर std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_b, अचर RTLIL::SigSpec &sig_s, अचर std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_s, अचर std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_s, अचर std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर RTLIL::SigBit &sig_d, अचर std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, अचर RTLIL::SigBit &sig_a, अचर RTLIL::SigBit &sig_b, अचर RTLIL::SigBit &sig_c, अचर RTLIL::SigBit &sig_d, अचर std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, पूर्णाङ्क width = 1, अचर std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, पूर्णाङ्क width = 1, अचर std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, पूर्णाङ्क width = 1, अचर std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, पूर्णाङ्क width = 1, अचर std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, अचर std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, अचर std::string &tag, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_c, अचर std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, अचर std::string &tag, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_c, अचर RTLIL::SigSpec &sig_y, अचर std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, अचर std::string &tag, अचर RTLIL::SigSpec &sig_a, अचर std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, अचर std::string &tag, अचर RTLIL::SigSpec &sig_a, अचर RTLIL::SigSpec &sig_s, अचर RTLIL::SigSpec &sig_c, अचर std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, अचर std::string &tag, अचर RTLIL::SigSpec &sig_a, अचर std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, अचर RTLIL::SigSpec &sig_e, अचर std::string &src = "");
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

	std::string to_rtlil_str() अचर;
#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णाङ्क, RTLIL::Module*> *get_all_modules(शून्य);
#endif
};

संरचना RTLIL::Wire : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

रक्षित:
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
	पूर्णाङ्क width, start_offset, port_id;
	bool port_आगत, port_output, upto, is_signed;

	RTLIL::Design *design() अचर { फर्काऊ module ? module->design : nullptr; }
	TwinePool &twines() अचर { फर्काऊ module->twines(); }

	bool known_driver() अचर { फर्काऊ driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() अचर    { log_assert(driverCell_); फर्काऊ driverCell_; };
	RTLIL::IdString driverPort() अचर { log_assert(driverCell_); फर्काऊ driverPort_; };

	पूर्णाङ्क from_hdl_index(पूर्णाङ्क hdl_index) {
		पूर्णाङ्क zero_index = hdl_index - start_offset;
		पूर्णाङ्क rtlil_index = upto ? width - 1 - zero_index : zero_index;
		फर्काऊ rtlil_index >= 0 && rtlil_index < width ? rtlil_index : पूर्णांक_न्यून;
	}

	पूर्णाङ्क to_hdl_index(पूर्णाङ्क rtlil_index) {
		यदि (rtlil_index < 0 || rtlil_index >= width)
			फर्काऊ पूर्णांक_न्यून;
		पूर्णाङ्क zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		फर्काऊ zero_index + start_offset;
	}

	std::string to_rtlil_str() अचर;

#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णाङ्क, RTLIL::Wire*> *get_all_wires(शून्य);
#endif
};

अन्तःस्थ पूर्णाङ्क GetSize(RTLIL::Wire *wire) {
	फर्काऊ wire->width;
}

संरचना RTLIL::Memory : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() अचर { फर्काऊ module ? module->design : nullptr; }
	TwinePool &twines() अचर { फर्काऊ module->twines(); }

	RTLIL::MemoryNameId name;

	पूर्णाङ्क width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णाङ्क, RTLIL::Memory*> *get_all_memorys(शून्य);
#endif

	std::string to_rtlil_str() अचर;
};

संरचना RTLIL::Cell : सार्वजनिक RTLIL::AttrObject
{
गुप्त:
	शून्य initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &सङ्केत, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
सार्वजनिक:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

रक्षित:
	// use module->addCell() and module->remove() to create or destroy cells
	मित्र संरचना RTLIL::Module;
	Cell();
	~Cell();

सार्वजनिक:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	शून्य चालक=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() अचर { फर्काऊ module ? module->design : nullptr; }
	TwinePool &twines() अचर { फर्काऊ module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) अचर;
	शून्य unsetPort(RTLIL::IdString portname);
	शून्य setPort(RTLIL::IdString portname, RTLIL::SigSpec सङ्केत);
	ढाँचा<typename N, YS_NAME_STRING(N)> शून्य setPort(N portname, RTLIL::SigSpec सङ्केत)
		{ setPort(module->twines().add(std::move(portname)), std::move(सङ्केत)); }
	अचर RTLIL::SigSpec &getPort(RTLIL::IdString portname) अचर;
	अचर dict<RTLIL::IdString, RTLIL::SigSpec> &connections() अचर;

	bool known() अचर;
	bool आगत(RTLIL::IdString portname) अचर;
	bool output(RTLIL::IdString portname) अचर;
	PortDir port_dir(RTLIL::IdString portname) अचर;

	bool hasParam(RTLIL::IdString paramname) अचर;
	शून्य unsetParam(RTLIL::IdString paramname);
	शून्य setParam(RTLIL::IdString paramname, RTLIL::Const मान);
	अचर RTLIL::Const &getParam(RTLIL::IdString paramname) अचर;

	ढाँचा<typename N, YS_NAME_STRING(N)> bool hasParam(N name) अचर
		{ फर्काऊ hasParam(module->twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> शून्य unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> शून्य setParam(N name, RTLIL::Const मान)
		{ setParam(module->twines().add(std::move(name)), std::move(मान)); }
	ढाँचा<typename N, YS_NAME_STRING(N)> अचर RTLIL::Const &getParam(N name) अचर
		{ फर्काऊ getParam(module->twines().find(std::string_view(name))); }

	शून्य sort();
	शून्य check();
	शून्य fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() अचर;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णाङ्क, RTLIL::Cell*> *get_all_cells(शून्य);
#endif

	bool has_memid() अचर;
	bool is_mem_cell() अचर;
	bool is_builtin_ff() अचर;

	std::string to_rtlil_str() अचर;
};

संरचना RTLIL::CaseRule : सार्वजनिक RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() अचर;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() अचर;
};

संरचना RTLIL::SwitchRule : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::SigSpec सङ्केत;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() अचर;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() अचर;
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

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() अचर;
};

संरचना RTLIL::Process : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(hashidx_); फर्काऊ h; }

रक्षित:
	// use module->addProcess() and module->remove() to create or destroy processes
	मित्र संरचना RTLIL::Module;
	Process();
	~Process();

सार्वजनिक:
	RTLIL::Module *module;
	RTLIL::CaseRule जरा_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() अचर { फर्काऊ module ? module->design : nullptr; }
	TwinePool &twines() अचर { फर्काऊ module->twines(); }

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() अचर;

	std::string to_rtlil_str() अचर;
};

अन्तःस्थ RTLIL::SigBit::SigBit() : wire(रिक्त), data(RTLIL::State::S0) { }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(रिक्त), data(bit) { }
अन्तःस्थ RTLIL::SigBit::SigBit(bool bit) : wire(रिक्त), data(bit ? State::S1 : State::S0) { }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, पूर्णाङ्क offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
अन्तःस्थ RTLIL::SigBit::SigBit(अचर RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); यदि (wire) offset = chunk.offset; अन्यथा data = chunk.data[0]; }
अन्तःस्थ RTLIL::SigBit::SigBit(अचर RTLIL::SigChunk &chunk, पूर्णाङ्क index) : wire(chunk.wire) { यदि (wire) offset = chunk.offset + index; अन्यथा data = chunk.data[index]; }

अन्तःस्थ bool RTLIL::SigBit::चालक<(अचर RTLIL::SigBit &other) अचर {
	यदि (wire == other.wire)
		फर्काऊ wire ? (offset < other.offset) : (data < other.data);
	यदि (wire != nullptr && other.wire != nullptr)
		फर्काऊ wire->name < other.wire->name;
	फर्काऊ (wire != nullptr) < (other.wire != nullptr);
}

अन्तःस्थ bool RTLIL::SigBit::चालक==(अचर RTLIL::SigBit &other) अचर {
	फर्काऊ (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

अन्तःस्थ bool RTLIL::SigBit::चालक!=(अचर RTLIL::SigBit &other) अचर {
	फर्काऊ (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

अन्तःस्थ Hasher RTLIL::SigBit::hash_into(Hasher h) अचर {
	यदि (wire) {
		h.eat(offset);
		h.eat(wire->name);
		फर्काऊ h;
	}
	h.eat(data);
	फर्काऊ h;
}


अन्तःस्थ Hasher RTLIL::SigBit::hash_शीर्ष() अचर {
	Hasher h;
	यदि (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		फर्काऊ h;
	}
	h.force(data);
	फर्काऊ h;
}

अन्तःस्थ RTLIL::SigBit &RTLIL::SigSpecIterator::चालक*() अचर {
	फर्काऊ (*sig_p)[index];
}

अन्तःस्थ अचर RTLIL::SigBit &RTLIL::SigSpecConstIterator::चालक*() {
	bit = (*sig_p)[index];
	फर्काऊ bit;
}

अन्तःस्थ RTLIL::SigBit::SigBit(अचर RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	स्वतः it = sig.chunks().begin();
	*this = SigBit(*it);
}

ढाँचा<typename T>
शून्य RTLIL::Module::rewrite_sigspecs(T &functor)
{
	लागि (स्वतः &it : cells_)
		it.second->rewrite_sigspecs(functor);
	लागि (स्वतः &it : processes)
		it.second->rewrite_sigspecs(functor);
	लागि (स्वतः &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

ढाँचा<typename T>
शून्य RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	लागि (स्वतः &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	लागि (स्वतः &it : processes)
		it.second->rewrite_sigspecs2(functor);
	लागि (स्वतः &it : connections_) {
		functor(it.first, it.second);
	}
}

ढाँचा<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs(T &functor) {
	लागि (स्वतः &it : connections_)
		functor(it.second);
}

ढाँचा<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	लागि (स्वतः &it : connections_)
		functor(it.second);
}

ढाँचा<typename T>
शून्य RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	लागि (स्वतः &it : compare)
		functor(it);
	लागि (स्वतः &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	लागि (स्वतः it : switches)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
शून्य RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	लागि (स्वतः &it : compare)
		functor(it);
	लागि (स्वतः &it : actions) {
		functor(it.first, it.second);
	}
	लागि (स्वतः it : switches)
		it->rewrite_sigspecs2(functor);
}

ढाँचा<typename T>
शून्य RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(सङ्केत);
	लागि (स्वतः it : cases)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
शून्य RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(सङ्केत);
	लागि (स्वतः it : cases)
		it->rewrite_sigspecs2(functor);
}

ढाँचा<typename T>
शून्य RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(सङ्केत);
	लागि (स्वतः &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	लागि (स्वतः &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

ढाँचा<typename T>
शून्य RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(सङ्केत);
	लागि (स्वतः &it : actions) {
		functor(it.first, it.second);
	}
	लागि (स्वतः &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

ढाँचा<typename T>
शून्य RTLIL::Process::rewrite_sigspecs(T &functor)
{
	जरा_case.rewrite_sigspecs(functor);
	लागि (स्वतः it : syncs)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
शून्य RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	जरा_case.rewrite_sigspecs2(functor);
	लागि (स्वतः it : syncs)
		it->rewrite_sigspecs2(functor);
}

#अन्तर्भाव "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
