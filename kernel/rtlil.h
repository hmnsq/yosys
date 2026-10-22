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

#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/twine.h"

#അന്തര്ഭാവ <deque>
#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	എണ്ണൽ State : unsigned അക്ഷരം {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	എണ്ണൽ SyncType : unsigned അക്ഷരം {
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
	എണ്ണൽ ConstFlags : unsigned അക്ഷരം {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	എണ്ണൽ SelectPartials : unsigned അക്ഷരം {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	എണ്ണൽ SelectBoxes : unsigned അക്ഷരം {
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

	എണ്ണൽ PortDir : unsigned അക്ഷരം  {
		PD_UNKNOWN = 0,
		PD_നിവേശം = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr പൂർണ്ണസംഖ്യ WIDTH_LIMIT = 1 << 30;

	ഘടന Const;
	ഘടന AttrObject;
	ഘടന SelectedMember;
	ഘടന Selection;
	ഘടന Monitor;
	ഘടന Design;
	ഘടന Module;
	ഘടന Wire;
	ഘടന Memory;
	ഘടന Cell;
	ഘടന SigChunk;
	ഘടന SigBit;
	ഘടന SigSpecIterator;
	ഘടന SigSpecConstIterator;
	ഘടന SigSpec;
	ഘടന CaseRule;
	ഘടന SwitchRule;
	ഘടന MemWriteAction;
	ഘടന SyncRule;
	ഘടന Process;

	തരനിർവചനം std::pair<SigSpec, SigSpec> SigSig;
};

ഘടന SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	ശൂന്യം copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			സ്ഥിരം dict<IdString, RTLIL::Const> &src,
			സ്ഥിരം RTLIL::Design *src_design, RTLIL::Design *dst_design);

	ബാഹ്യ dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	സ്ഥിരം pool<IdString> &builtin_ff_cell_types();

	സ്ഥിതം വരിയിൽ std::string escape_id(സ്ഥിരം std::string &str) {
		എങ്കിൽ (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			തിരികെ "\\" + str;
		തിരികെ str;
	}

	സ്ഥിതം വരിയിൽ std::string unescape_id(സ്ഥിരം std::string &str) {
		എങ്കിൽ (str.size() < 2)
			തിരികെ str;
		എങ്കിൽ (str[0] != '\\')
			തിരികെ str;
		എങ്കിൽ (str[1] == '$' || str[1] == '\\')
			തിരികെ str;
		എങ്കിൽ (str[1] >= '0' && str[1] <= '9')
			തിരികെ str;
		തിരികെ str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> ഘടന sort_by_name_str {
		bool operator()(T *a, T *b) സ്ഥിരം {
			തിരികെ a->name.lt_by_name(b->name);
		}
	};

	ഘടന sort_by_id_str {
		സ്ഥിരം TwinePool& pool;
		explicit sort_by_id_str(സ്ഥിരം TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) സ്ഥിരം {
			തിരികെ pool.compare_by_name(a, b) < 0;
		}
	};

	സ്ഥിതം വരിയിൽ std::string encode_filename(സ്ഥിരം std::string &filename)
	{
		std::stringstream val;
		എങ്കിൽ (!std::any_of(filename.begin(), filename.end(), [](അക്ഷരം c) {
			തിരികെ static_cast<unsigned അക്ഷരം>(c) < 33 || static_cast<unsigned അക്ഷരം>(c) > 126;
		})) തിരികെ filename;
		വേണ്ടി (unsigned അക്ഷരം സ്ഥിരം c : filename) {
			എങ്കിൽ (c < 33 || c > 126)
				val << stringf("$%02x", c);
			അല്ലെങ്കിൽ
				val << c;
		}
		തിരികെ val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_and         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_or          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_xor         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_xnor        (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_reduce_and  (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_reduce_or   (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_reduce_xor  (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_reduce_xnor (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_reduce_bool (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_logic_not   (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_logic_and   (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_logic_or    (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_shl         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_shr         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_sshl        (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_sshr        (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_shift       (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_shiftx      (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_lt          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_le          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_eq          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_ne          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_eqx         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_nex         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_ge          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_gt          (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_add         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_sub         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_mul         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_div         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_divfloor    (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_modfloor    (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_mod         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_pow         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_pos         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_buf         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);
	RTLIL::Const const_neg         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, bool signed1, bool signed2, പൂർണ്ണസംഖ്യ result_len);

	RTLIL::Const const_mux         (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, സ്ഥിരം RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, സ്ഥിരം RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2);
	RTLIL::Const const_demux       (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, സ്ഥിരം RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename താക്കോൽ_2 = IdString>
	ഘടന ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using മൂല്യം_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<താക്കോൽ_2, T>::iterator it;
		dict<താക്കോൽ_2, T> *list_p;
		പൂർണ്ണസംഖ്യ *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, പൂർണ്ണസംഖ്യ *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			എങ്കിൽ (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} അല്ലെങ്കിൽ {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(സ്ഥിരം RTLIL::ObjIterator<T, താക്കോൽ_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			എങ്കിൽ (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(സ്ഥിരം RTLIL::ObjIterator<T, താക്കോൽ_2> &other) {
			എങ്കിൽ (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			എങ്കിൽ (refcount_p)
				(*refcount_p)++;
			തിരികെ *this;
		}

		~ObjIterator() {
			എങ്കിൽ (refcount_p)
				(*refcount_p)--;
		}

		വരിയിൽ T operator*() സ്ഥിരം {
			log_assert(list_p != nullptr);
			തിരികെ it->second;
		}

		വരിയിൽ bool operator!=(സ്ഥിരം RTLIL::ObjIterator<T, താക്കോൽ_2> &other) സ്ഥിരം {
			എങ്കിൽ (list_p == nullptr || other.list_p == nullptr)
				തിരികെ list_p != other.list_p;
			തിരികെ it != other.it;
		}


		വരിയിൽ bool operator==(സ്ഥിരം RTLIL::ObjIterator<T, താക്കോൽ_2> &other) സ്ഥിരം {
			തിരികെ !(*this != other);
		}

		വരിയിൽ ObjIterator<T, താക്കോൽ_2>& operator++() {
			log_assert(list_p != nullptr);
			എങ്കിൽ (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			തിരികെ *this;
		}

		വരിയിൽ ObjIterator<T, താക്കോൽ_2>& operator+=(പൂർണ്ണസംഖ്യ amt) {
			log_assert(list_p != nullptr);
			it += amt;
			എങ്കിൽ (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			തിരികെ *this;
		}

		വരിയിൽ ObjIterator<T, താക്കോൽ_2> operator+(പൂർണ്ണസംഖ്യ amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, താക്കോൽ_2> new_obj(*this);
			new_obj.it += amt;
			എങ്കിൽ (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			തിരികെ new_obj;
		}

		വരിയിൽ സ്ഥിരം ObjIterator<T, താക്കോൽ_2> operator++(പൂർണ്ണസംഖ്യ) {
			ObjIterator<T, താക്കോൽ_2> result(*this);
			++(*this);
			തിരികെ result;
		}
	};

	template<typename T, typename താക്കോൽ_2 = IdString>
	ഘടന ObjRange
	{
		dict<താക്കോൽ_2, T> *list_p;
		പൂർണ്ണസംഖ്യ *refcount_p;

		ObjRange(decltype(list_p) list_p, പൂർണ്ണസംഖ്യ *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, താക്കോൽ_2> begin() { തിരികെ RTLIL::ObjIterator<T, താക്കോൽ_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, താക്കോൽ_2> end() { തിരികെ RTLIL::ObjIterator<T, താക്കോൽ_2>(); }

		size_t size() സ്ഥിരം {
			തിരികെ list_p->size();
		}

		operator pool<T>() സ്ഥിരം {
			pool<T> result;
			വേണ്ടി (auto &it : *list_p)
				result.തിരുകുക(it.second);
			തിരികെ result;
		}

		operator std::vector<T>() സ്ഥിരം {
			std::vector<T> result;
			result.reserve(list_p->size());
			വേണ്ടി (auto &it : *list_p)
				result.തള്ളുക_back(it.second);
			തിരികെ result;
		}

		pool<T> to_pool() സ്ഥിരം { തിരികെ *this; }
		std::vector<T> to_vector() സ്ഥിരം { തിരികെ *this; }
	};
};

ഘടന RTLIL::Const
{
	short പൂർണ്ണസംഖ്യ flags;
സ്വകാര്യ:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	എണ്ണൽ class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	സംഘം {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() സ്ഥിരം { തിരികെ tag == backing_tag::bits; }
	bool is_str() സ്ഥിരം { തിരികെ tag == backing_tag::string; }

	bitvectype* get_if_bits() { തിരികെ is_bits() ? &bits_ : ശൂന്യ; }
	std::string* get_if_str() { തിരികെ is_str() ? &str_ : ശൂന്യ; }
	സ്ഥിരം bitvectype* get_if_bits() സ്ഥിരം { തിരികെ is_bits() ? &bits_ : ശൂന്യ; }
	സ്ഥിരം std::string* get_if_str() സ്ഥിരം { തിരികെ is_str() ? &str_ : ശൂന്യ; }

	bitvectype& get_bits();
	std::string& get_str();
	സ്ഥിരം bitvectype& get_bits() സ്ഥിരം;
	സ്ഥിരം std::string& get_str() സ്ഥിരം;
	std::vector<RTLIL::State>& bits_internal();
	ശൂന്യം bitvectorize_internal();

പൊതു:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, പൂർണ്ണസംഖ്യ width);
	Const(RTLIL::State bit, പൂർണ്ണസംഖ്യ width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(സ്ഥിരം std::vector<bool> &bits);
	Const(സ്ഥിരം RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(സ്ഥിരം RTLIL::Const &other);
	~Const();

	ഘടന Builder
	{
		Builder() {}
		Builder(പൂർണ്ണസംഖ്യ expected_width) { bits.reserve(expected_width); }
		ശൂന്യം തള്ളുക_back(RTLIL::State b) { bits.തള്ളുക_back(b); }
		പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ static_cast<പൂർണ്ണസംഖ്യ>(bits.size()); }
		Const build() { തിരികെ Const(std::move(bits)); }
	സ്വകാര്യ:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(സ്ഥിരം RTLIL::Const &other) സ്ഥിരം;
	bool operator ==(സ്ഥിരം RTLIL::Const &other) സ്ഥിരം;
	bool operator !=(സ്ഥിരം RTLIL::Const &other) സ്ഥിരം;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { തിരികെ bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	ശൂന്യം bitvectorize() സ്ഥിരം { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() സ്ഥിരം;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	പൂർണ്ണസംഖ്യ as_int(bool is_signed = false) സ്ഥിരം;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) സ്ഥിരം;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<പൂർണ്ണസംഖ്യ> try_as_int(bool is_signed = false) സ്ഥിരം;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	പൂർണ്ണസംഖ്യ as_int_saturating(bool is_signed = false) സ്ഥിരം;

	ശൂന്യം tag_bare_integer_const(സ്ഥിരം std::string &മൂല്യം);

	std::string as_string(സ്ഥിരം അക്ഷരം* any = "-") സ്ഥിരം;
	സ്ഥിതം Const from_string(സ്ഥിരം std::string &str);
	std::vector<RTLIL::State> to_bits() സ്ഥിരം;

	std::string decode_string() സ്ഥിരം;
	പൂർണ്ണസംഖ്യ size() സ്ഥിരം;
	bool empty() സ്ഥിരം;

	ശൂന്യം append(സ്ഥിരം RTLIL::Const &other);
	ശൂന്യം set(പൂർണ്ണസംഖ്യ i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	ശൂന്യം resize(പൂർണ്ണസംഖ്യ size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	സ്വകാര്യ:
		സ്ഥിരം Const* parent;
		size_t idx;

	പൊതു:
		using iterator_category = std::bidirectional_iterator_tag;
		using മൂല്യം_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = സ്ഥിരം State*;
		using reference = സ്ഥിരം State&;

		const_iterator(സ്ഥിരം Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() സ്ഥിരം;

		const_iterator& operator++() { ++idx; തിരികെ *this; }
		const_iterator& operator--() { --idx; തിരികെ *this; }
		const_iterator operator++(പൂർണ്ണസംഖ്യ) { const_iterator result(*this); ++idx; തിരികെ result; }
		const_iterator operator--(പൂർണ്ണസംഖ്യ) { const_iterator result(*this); --idx; തിരികെ result; }
		const_iterator& operator+=(പൂർണ്ണസംഖ്യ i) { idx += i; തിരികെ *this; }

		const_iterator operator+(പൂർണ്ണസംഖ്യ add) {
			തിരികെ const_iterator(*parent, idx + add);
		}
		const_iterator operator-(പൂർണ്ണസംഖ്യ sub) {
			തിരികെ const_iterator(*parent, idx - sub);
		}
		പൂർണ്ണസംഖ്യ operator-(സ്ഥിരം const_iterator& other) {
			തിരികെ idx - other.idx;
		}

		bool operator==(സ്ഥിരം const_iterator& other) സ്ഥിരം {
			തിരികെ idx == other.idx;
		}

		bool operator!=(സ്ഥിരം const_iterator& other) സ്ഥിരം {
			തിരികെ !(*this == other);
		}
	};

	class iterator {
	സ്വകാര്യ:
		Const* parent;
		size_t idx;

	പൊതു:
		class proxy {
		സ്വകാര്യ:
			Const* parent;
			size_t idx;
		പൊതു:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() സ്ഥിരം { തിരികെ (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); തിരികെ *this; }
			proxy& operator=(സ്ഥിരം proxy& other) { parent->set(idx, (*other.parent)[other.idx]); തിരികെ *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using മൂല്യം_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() സ്ഥിരം { തിരികെ proxy(parent, idx); }
		iterator& operator++() { ++idx; തിരികെ *this; }
		iterator& operator--() { --idx; തിരികെ *this; }
		iterator operator++(പൂർണ്ണസംഖ്യ) { iterator result(*this); ++idx; തിരികെ result; }
		iterator operator--(പൂർണ്ണസംഖ്യ) { iterator result(*this); --idx; തിരികെ result; }
		iterator& operator+=(പൂർണ്ണസംഖ്യ i) { idx += i; തിരികെ *this; }

		iterator operator+(പൂർണ്ണസംഖ്യ add) {
			തിരികെ iterator(*parent, idx + add);
		}
		iterator operator-(പൂർണ്ണസംഖ്യ sub) {
			തിരികെ iterator(*parent, idx - sub);
		}
		പൂർണ്ണസംഖ്യ operator-(സ്ഥിരം iterator& other) {
			തിരികെ idx - other.idx;
		}

		bool operator==(സ്ഥിരം iterator& other) സ്ഥിരം {
			തിരികെ idx == other.idx;
		}

		bool operator!=(സ്ഥിരം iterator& other) സ്ഥിരം {
			തിരികെ !(*this == other);
		}
	};

	const_iterator begin() സ്ഥിരം {
		തിരികെ const_iterator(*this, 0);
	}
	const_iterator end() സ്ഥിരം {
		തിരികെ const_iterator(*this, size());
	}
	iterator begin() {
		തിരികെ iterator(*this, 0);
	}
	iterator end() {
		തിരികെ iterator(*this, size());
	}
	State back() സ്ഥിരം {
		തിരികെ *(end() - 1);
	}
	State front() സ്ഥിരം {
		തിരികെ *begin();
	}
	State at(size_t i) സ്ഥിരം {
		തിരികെ *const_iterator(*this, i);
	}
	State operator[](size_t i) സ്ഥിരം {
		തിരികെ *const_iterator(*this, i);
	}

	bool is_fully_zero() സ്ഥിരം;
	bool is_fully_ones() സ്ഥിരം;
	bool is_fully_def() സ്ഥിരം;
	bool is_fully_undef() സ്ഥിരം;
	bool is_fully_undef_x_only() സ്ഥിരം;
	bool is_onehot(പൂർണ്ണസംഖ്യ *pos = nullptr) സ്ഥിരം;

	RTLIL::Const extract(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ len = 1, RTLIL::State padding = RTLIL::State::S0) സ്ഥിരം;

	// find the MSB without redundant leading bits
	പൂർണ്ണസംഖ്യ get_min_size(bool is_signed) സ്ഥിരം;

	// compress representation to the minimum required bits
	ശൂന്യം compress(bool is_signed = false);

	std::optional<പൂർണ്ണസംഖ്യ> as_int_compress(bool is_signed) സ്ഥിരം;

	ശൂന്യം extu(പൂർണ്ണസംഖ്യ width) {
		resize(width, RTLIL::State::S0);
	}

	ശൂന്യം exts(പൂർണ്ണസംഖ്യ width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};

ഘടന RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) സ്ഥിരം;

	ശൂന്യം set_bool_attribute(RTLIL::IdString id, bool മൂല്യം=true);
	bool get_bool_attribute(RTLIL::IdString id) സ്ഥിരം;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) സ്ഥിരം {
		തിരികെ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ശൂന്യം set_string_attribute(RTLIL::IdString id, string മൂല്യം);
	string get_string_attribute(RTLIL::IdString id) സ്ഥിരം;

	ശൂന്യം set_strpool_attribute(RTLIL::IdString id, സ്ഥിരം pool<string> &data);
	ശൂന്യം add_strpool_attribute(RTLIL::IdString id, സ്ഥിരം pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) സ്ഥിരം;

	ശൂന്യം set_src_attribute(സ്ഥിരം std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() സ്ഥിരം {
		തിരികെ get_string_attribute(ID::src);
	}

	ശൂന്യം set_hdlname_attribute(സ്ഥിരം vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() സ്ഥിരം;

	ശൂന്യം set_intvec_attribute(RTLIL::IdString id, സ്ഥിരം vector<പൂർണ്ണസംഖ്യ> &data);
	vector<പൂർണ്ണസംഖ്യ> get_intvec_attribute(RTLIL::IdString id) സ്ഥിരം;
};

#അന്തര്ഭാവ "kernel/rtlil_twine_compat.h"

ഘടന RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

ഘടന RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	പൂർണ്ണസംഖ്യ width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(സ്ഥിരം RTLIL::Const &മൂല്യം) : wire(nullptr), data(മൂല്യം.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&മൂല്യം) : wire(nullptr), data(മൂല്യം.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(സ്ഥിരം std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(പൂർണ്ണസംഖ്യ val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(പൂർണ്ണസംഖ്യ val, പൂർണ്ണസംഖ്യ width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, പൂർണ്ണസംഖ്യ width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(സ്ഥിരം RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ length) സ്ഥിരം;
	RTLIL::SigBit operator[](പൂർണ്ണസംഖ്യ offset) സ്ഥിരം;
	വരിയിൽ പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ width; }
	വരിയിൽ bool is_wire() സ്ഥിരം { തിരികെ wire != ശൂന്യ; }

	bool operator <(സ്ഥിരം RTLIL::SigChunk &other) സ്ഥിരം;
	bool operator ==(സ്ഥിരം RTLIL::SigChunk &other) സ്ഥിരം;
	bool operator !=(സ്ഥിരം RTLIL::SigChunk &other) സ്ഥിരം;
};

ഘടന RTLIL::SigBit
{
	RTLIL::Wire *wire;
	സംഘം {
		RTLIL::State data; // used if wire == NULL
		പൂർണ്ണസംഖ്യ offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, പൂർണ്ണസംഖ്യ offset);
	SigBit(സ്ഥിരം RTLIL::SigChunk &chunk);
	SigBit(സ്ഥിരം RTLIL::SigChunk &chunk, പൂർണ്ണസംഖ്യ index);
	SigBit(സ്ഥിരം RTLIL::SigSpec &sig);
	SigBit(സ്ഥിരം RTLIL::SigBit &sigbit) = സ്ഥിരസ്ഥിതി;
	RTLIL::SigBit &operator =(സ്ഥിരം RTLIL::SigBit &other) = സ്ഥിരസ്ഥിതി;

	വരിയിൽ bool is_wire() സ്ഥിരം { തിരികെ wire != ശൂന്യ; }

	bool operator <(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം;
	bool operator ==(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം;
	bool operator !=(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
	[[nodiscard]] Hasher hash_മുകൾ() സ്ഥിരം;
};

namespace hashlib {
	template <>
	ഘടന hash_ops<RTLIL::SigBit> {
		സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം RTLIL::SigBit &a, സ്ഥിരം RTLIL::SigBit &b) {
			തിരികെ a == b;
		}
		[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash(സ്ഥിരം RTLIL::SigBit sb) {
			തിരികെ sb.hash_മുകൾ();
		}
		[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം RTLIL::SigBit sb, Hasher h) {
			തിരികെ sb.hash_into(h);
		}
	};
};

ഘടന RTLIL::SigSpecIterator
{
	തരനിർവചനം std::നിവേശം_iterator_tag iterator_category;
	തരനിർവചനം RTLIL::SigBit മൂല്യം_type;
	തരനിർവചനം ptrdiff_t difference_type;
	തരനിർവചനം RTLIL::SigBit* pointer;
	തരനിർവചനം RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	പൂർണ്ണസംഖ്യ index;

	വരിയിൽ RTLIL::SigBit &operator*() സ്ഥിരം;
	വരിയിൽ bool operator!=(സ്ഥിരം RTLIL::SigSpecIterator &other) സ്ഥിരം { തിരികെ index != other.index; }
	വരിയിൽ bool operator==(സ്ഥിരം RTLIL::SigSpecIterator &other) സ്ഥിരം { തിരികെ index == other.index; }
	വരിയിൽ ശൂന്യം operator++() { index++; }
};

ഘടന RTLIL::SigSpecConstIterator
{
	തരനിർവചനം std::നിവേശം_iterator_tag iterator_category;
	തരനിർവചനം RTLIL::SigBit മൂല്യം_type;
	തരനിർവചനം ptrdiff_t difference_type;
	തരനിർവചനം RTLIL::SigBit* pointer;
	തരനിർവചനം RTLIL::SigBit& reference;

	സ്ഥിരം RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	പൂർണ്ണസംഖ്യ index;

	വരിയിൽ സ്ഥിരം RTLIL::SigBit &operator*();
	വരിയിൽ bool operator!=(സ്ഥിരം RTLIL::SigSpecConstIterator &other) സ്ഥിരം { തിരികെ index != other.index; }
	വരിയിൽ bool operator==(സ്ഥിരം RTLIL::SigSpecIterator &other) സ്ഥിരം { തിരികെ index == other.index; }
	വരിയിൽ ശൂന്യം operator++() { index++; }
};

ഘടന RTLIL::SigSpec
{
സ്വകാര്യ:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	എണ്ണൽ Representation : അക്ഷരം {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	ഘടന AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(സ്ഥിരം AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(സ്ഥിരം AtomicHash &rhs) { store(rhs.load()); തിരികെ *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> പഠ() സ്ഥിരം {
			Hasher::hash_t മൂല്യം = load();
			എങ്കിൽ (മൂല്യം == 0)
				തിരികെ std::nullopt;
			തിരികെ മൂല്യം;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		ശൂന്യം set(Hasher::hash_t മൂല്യം) സ്ഥിരം {
			log_assert(മൂല്യം != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(മൂല്യം, std::memory_order_relaxed);
			log_assert(old == 0 || old == മൂല്യം);
		}
		ശൂന്യം clear() { store(0); }
	സ്വകാര്യ:
		പൂർണ്ണസംഖ്യ load() സ്ഥിരം { തിരികെ atomic_.load(std::memory_order_relaxed); }
		ശൂന്യം store(Hasher::hash_t മൂല്യം) സ്ഥിരം {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(മൂല്യം, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	സംഘം {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	ശൂന്യം init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	ശൂന്യം unpack();
	വരിയിൽ ശൂന്യം inline_unpack() {
		എങ്കിൽ (rep_ == CHUNK)
			unpack();
	}
	ശൂന്യം try_repack();

	Hasher::hash_t updhash() സ്ഥിരം;
	ശൂന്യം destroy() {
		എങ്കിൽ (rep_ == CHUNK)
			chunk_.~SigChunk();
		അല്ലെങ്കിൽ
			bits_.~vector();
	}
	friend ഘടന Chunks;

പൊതു:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(സ്ഥിരം SigSpec &മൂല്യം) : rep_(മൂല്യം.rep_), hash_(മൂല്യം.hash_) {
		എങ്കിൽ (മൂല്യം.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(മൂല്യം.chunk_);
		അല്ലെങ്കിൽ
			new (&bits_) std::vector<RTLIL::SigBit>(മൂല്യം.bits_);
	}
	SigSpec(SigSpec &&മൂല്യം) : rep_(മൂല്യം.rep_), hash_(മൂല്യം.hash_) {
		എങ്കിൽ (മൂല്യം.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(മൂല്യം.chunk_));
		അല്ലെങ്കിൽ
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(മൂല്യം.bits_));
	}
	SigSpec(സ്ഥിരം RTLIL::Const &മൂല്യം);
	SigSpec(RTLIL::Const &&മൂല്യം);
	SigSpec(സ്ഥിരം RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ width = 1);
	SigSpec(സ്ഥിരം std::string &str);
	SigSpec(പൂർണ്ണസംഖ്യ val, പൂർണ്ണസംഖ്യ width = 32);
	SigSpec(RTLIL::State bit, പൂർണ്ണസംഖ്യ width = 1);
	SigSpec(സ്ഥിരം RTLIL::SigBit &bit, പൂർണ്ണസംഖ്യ width = 1);
	SigSpec(സ്ഥിരം std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(സ്ഥിരം std::vector<RTLIL::SigBit> &bits);
	SigSpec(സ്ഥിരം pool<RTLIL::SigBit> &bits);
	SigSpec(സ്ഥിരം std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(സ്ഥിരം SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		എങ്കിൽ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		അല്ലെങ്കിൽ
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		തിരികെ *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		എങ്കിൽ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		അല്ലെങ്കിൽ
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		തിരികെ *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	ഘടന Chunks {
		Chunks(സ്ഥിരം SigSpec &spec) : spec(spec) {}
		ഘടന const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using മൂല്യം_type = സ്ഥിരം SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = സ്ഥിരം SigChunk *;
			using reference = സ്ഥിരം SigChunk &;

			സ്ഥിരം SigSpec &spec;
			പൂർണ്ണസംഖ്യ bit_index;
			SigChunk chunk;

			const_iterator(സ്ഥിരം SigSpec &spec) : spec(spec) {
				bit_index = 0;
				എങ്കിൽ (spec.rep_ == BITS)
					next_chunk_bits();
			}
			എണ്ണൽ End { END };
			const_iterator(സ്ഥിരം SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			ശൂന്യം next_chunk_bits();

			സ്ഥിരം SigChunk &operator*() {
				എങ്കിൽ (spec.rep_ == CHUNK)
					തിരികെ spec.chunk_;
				തിരികെ chunk;
			};
			സ്ഥിരം SigChunk *operator->() { തിരികെ &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				എങ്കിൽ (spec.rep_ == BITS)
					next_chunk_bits();
				തിരികെ *this;
			}
			bool operator==(സ്ഥിരം const_iterator &rhs) സ്ഥിരം { തിരികെ bit_index == rhs.bit_index; }
			bool operator!=(സ്ഥിരം const_iterator &rhs) സ്ഥിരം { തിരികെ !(*this == rhs); }
		};
		const_iterator begin() സ്ഥിരം { തിരികെ const_iterator(spec); }
		const_iterator end() സ്ഥിരം {
			const_iterator it(spec, const_iterator::END);
			തിരികെ it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			തിരികെ chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			തിരികെ chunk_vector.rend();
		}
		പൂർണ്ണസംഖ്യ size() {
			ensure_chunk_vector();
			തിരികെ chunk_vector.size();
		}
		പൂർണ്ണസംഖ്യ size() സ്ഥിരം {
			തിരികെ std::distance(begin(), end());
		}
		സ്ഥിരം SigChunk &at(പൂർണ്ണസംഖ്യ index) {
			ensure_chunk_vector();
			തിരികെ chunk_vector.at(index);
		}
		operator സ്ഥിരം std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			തിരികെ chunk_vector;
		}
	സ്വകാര്യ:
		ശൂന്യം ensure_chunk_vector() {
			എങ്കിൽ (spec.size() > 0 && chunk_vector.empty()) {
				വേണ്ടി (സ്ഥിരം RTLIL::SigChunk &c : *this)
					chunk_vector.തള്ളുക_back(c);
			}
		}
		സ്ഥിരം SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend ഘടന Chunks::const_iterator;

	വരിയിൽ Chunks chunks() സ്ഥിരം { തിരികെ {*this}; }
	വരിയിൽ സ്ഥിരം SigSpec &bits() സ്ഥിരം { തിരികെ *this; }

	വരിയിൽ പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	വരിയിൽ bool empty() സ്ഥിരം { തിരികെ size() == 0; };

	വരിയിൽ RTLIL::SigBit &operator[](പൂർണ്ണസംഖ്യ index) { inline_unpack(); hash_.clear(); തിരികെ bits_.at(index); }
	വരിയിൽ RTLIL::SigBit operator[](പൂർണ്ണസംഖ്യ index) സ്ഥിരം {
		എങ്കിൽ (rep_ == CHUNK) {
			എങ്കിൽ (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			എങ്കിൽ (chunk_.wire)
				തിരികെ RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			തിരികെ RTLIL::SigBit(chunk_.data[index]);
		}
		തിരികെ bits_.at(index);
	}

	വരിയിൽ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; തിരികെ it; }
	വരിയിൽ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); തിരികെ it; }

	വരിയിൽ RTLIL::SigSpecConstIterator begin() സ്ഥിരം { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; തിരികെ it; }
	വരിയിൽ RTLIL::SigSpecConstIterator end() സ്ഥിരം { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); തിരികെ it; }

	ശൂന്യം sort();
	ശൂന്യം sort_and_unify();

	ശൂന്യം replace(സ്ഥിരം RTLIL::SigSpec &pattern, സ്ഥിരം RTLIL::SigSpec &with);
	ശൂന്യം replace(സ്ഥിരം RTLIL::SigSpec &pattern, സ്ഥിരം RTLIL::SigSpec &with, RTLIL::SigSpec *other) സ്ഥിരം;

	ശൂന്യം replace(സ്ഥിരം dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	ശൂന്യം replace(സ്ഥിരം dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) സ്ഥിരം;

	ശൂന്യം replace(സ്ഥിരം std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	ശൂന്യം replace(സ്ഥിരം std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) സ്ഥിരം;

	ശൂന്യം replace(പൂർണ്ണസംഖ്യ offset, സ്ഥിരം RTLIL::SigSpec &with);

	ശൂന്യം remove(സ്ഥിരം RTLIL::SigSpec &pattern);
	ശൂന്യം remove(സ്ഥിരം RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) സ്ഥിരം;
	ശൂന്യം remove2(സ്ഥിരം RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	ശൂന്യം remove(സ്ഥിരം pool<RTLIL::SigBit> &pattern);
	ശൂന്യം remove(സ്ഥിരം pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) സ്ഥിരം;
	ശൂന്യം remove2(സ്ഥിരം pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	ശൂന്യം remove2(സ്ഥിരം std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	ശൂന്യം remove2(സ്ഥിരം pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	ശൂന്യം remove(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ length = 1);
	ശൂന്യം remove_const();

	RTLIL::SigSpec extract(സ്ഥിരം RTLIL::SigSpec &pattern, സ്ഥിരം RTLIL::SigSpec *other = ശൂന്യ) സ്ഥിരം;
	RTLIL::SigSpec extract(സ്ഥിരം pool<RTLIL::SigBit> &pattern, സ്ഥിരം RTLIL::SigSpec *other = ശൂന്യ) സ്ഥിരം;
	RTLIL::SigSpec extract(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ length = 1) സ്ഥിരം;
	RTLIL::SigSpec extract_end(പൂർണ്ണസംഖ്യ offset) സ്ഥിരം { തിരികെ extract(offset, size() - offset); }

	ശൂന്യം rewrite_wires(std::function<ശൂന്യം(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() സ്ഥിരം { log_assert(size()); തിരികെ (*this)[0]; };
	RTLIL::SigBit msb() സ്ഥിരം { log_assert(size()); തിരികെ (*this)[size() - 1]; };
	RTLIL::SigBit front() സ്ഥിരം { തിരികെ (*this)[0]; }
	RTLIL::SigBit back() സ്ഥിരം { തിരികെ (*this)[size() - 1]; }

	ശൂന്യം append(സ്ഥിരം RTLIL::SigSpec &സൂചന);
	വരിയിൽ ശൂന്യം append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	വരിയിൽ ശൂന്യം append(സ്ഥിരം RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	വരിയിൽ ശൂന്യം append(സ്ഥിരം RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	ശൂന്യം append(സ്ഥിരം RTLIL::SigBit &bit);
	വരിയിൽ ശൂന്യം append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	വരിയിൽ ശൂന്യം append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	ശൂന്യം extend_u0(പൂർണ്ണസംഖ്യ width, bool is_signed = false);

	RTLIL::SigSpec repeat(പൂർണ്ണസംഖ്യ അക്കം) സ്ഥിരം;

	ശൂന്യം reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം;
	bool operator ==(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം;
	വരിയിൽ bool operator !=(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം { തിരികെ !(*this == other); }

	bool is_wire() സ്ഥിരം;
	bool is_chunk() സ്ഥിരം;
	വരിയിൽ bool is_bit() സ്ഥിരം { തിരികെ size() == 1; }

	bool known_driver() സ്ഥിരം;

	bool is_fully_const() സ്ഥിരം;
	bool is_fully_zero() സ്ഥിരം;
	bool is_fully_ones() സ്ഥിരം;
	bool is_fully_def() സ്ഥിരം;
	bool is_fully_undef() സ്ഥിരം;
	bool has_const() സ്ഥിരം;
	bool has_const(State state) സ്ഥിരം;
	bool has_marked_bits() സ്ഥിരം;
	bool is_onehot(പൂർണ്ണസംഖ്യ *pos = nullptr) സ്ഥിരം;

	bool as_bool() സ്ഥിരം;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	പൂർണ്ണസംഖ്യ as_int(bool is_signed = false) സ്ഥിരം;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) സ്ഥിരം;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<പൂർണ്ണസംഖ്യ> try_as_int(bool is_signed = false) സ്ഥിരം;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	പൂർണ്ണസംഖ്യ as_int_saturating(bool is_signed = false) സ്ഥിരം;

	std::string as_string() സ്ഥിരം;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() സ്ഥിരം;
	RTLIL::Const as_const() സ്ഥിരം;
	RTLIL::Wire *as_wire() സ്ഥിരം;
	RTLIL::SigChunk as_chunk() സ്ഥിരം;
	RTLIL::SigBit as_bit() സ്ഥിരം;

	bool match(സ്ഥിരം അക്ഷരം* pattern) സ്ഥിരം;

	std::set<RTLIL::SigBit> to_sigbit_set() സ്ഥിരം;
	pool<RTLIL::SigBit> to_sigbit_pool() സ്ഥിരം;
	std::vector<RTLIL::SigBit> to_sigbit_vector() സ്ഥിരം;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം;

	സ്ഥിതം bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	സ്ഥിതം bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	സ്ഥിതം bool parse_rhs(സ്ഥിരം RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() സ്ഥിരം;
	operator std::vector<RTLIL::SigBit>() സ്ഥിരം { തിരികെ to_sigbit_vector(); }
	സ്ഥിരം RTLIL::SigBit &at(പൂർണ്ണസംഖ്യ offset, സ്ഥിരം RTLIL::SigBit &defval) { തിരികെ offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(പൂർണ്ണസംഖ്യ offset) { തിരികെ (*this)[offset]; }
	RTLIL::SigBit at(പൂർണ്ണസംഖ്യ offset) സ്ഥിരം { തിരികെ (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
		Hasher::hash_t val;
		എങ്കിൽ (std::optional<Hasher::hash_t> current = hash_.പഠ())
			val = *current;
		അല്ലെങ്കിൽ
			val = updhash();
		h.eat(val);
		തിരികെ h;
	}

#ifndef NDEBUG
	ശൂന്യം check(സ്ഥിരം Module *mod = nullptr) സ്ഥിരം;
#അല്ലെങ്കിൽ
	ശൂന്യം check(സ്ഥിരം Module *mod = nullptr) സ്ഥിരം { (ശൂന്യം)mod; }
#endif
};

ഘടന RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) സ്ഥിരം;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) സ്ഥിരം;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) സ്ഥിരം;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) സ്ഥിരം;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	ശൂന്യം optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() സ്ഥിരം {
		തിരികെ full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> ശൂന്യം select(T1 *module) {
		എങ്കിൽ (!selects_all() && selected_modules.എണ്ണം(module->name) == 0) {
			IdString name = module->name;
			selected_modules.തിരുകുക(name);
			selected_members.erase(name);
			എങ്കിൽ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> ശൂന്യം select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> ശൂന്യം select(T1 *module, RTLIL::IdString member) {
		എങ്കിൽ (!selects_all() && selected_modules.എണ്ണം(module->name.ref()) == 0) {
			selected_members[module->name.ref()].തിരുകുക(member);
			എങ്കിൽ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() സ്ഥിരം {
		തിരികെ !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	ശൂന്യം clear();

	// create a new selection which is empty
	സ്ഥിതം Selection EmptySelection(RTLIL::Design *design = nullptr) { തിരികെ Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	സ്ഥിതം Selection FullSelection(RTLIL::Design *design = nullptr) { തിരികെ Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	സ്ഥിതം Selection CompleteSelection(RTLIL::Design *design = nullptr) { തിരികെ Selection(true, true, design); };
};

ഘടന RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

	Monitor() {
		സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
		hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
		hashidx_ = hashidx_എണ്ണം;
	}

	മായാ ~Monitor() { }
	മായാ ശൂന്യം notify_module_add(RTLIL::Module*) { }
	മായാ ശൂന്യം notify_module_del(RTLIL::Module*) { }
	മായാ ശൂന്യം notify_connect(RTLIL::Cell*, RTLIL::IdString, സ്ഥിരം RTLIL::SigSpec&, സ്ഥിരം RTLIL::SigSpec&) { }
	മായാ ശൂന്യം notify_connect(RTLIL::Module*, സ്ഥിരം RTLIL::SigSig&) { }
	മായാ ശൂന്യം notify_connect(RTLIL::Module*, സ്ഥിരം std::vector<RTLIL::SigSig>&) { }
	മായാ ശൂന്യം notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
ഘടന define_map_t;

template<typename N>
വരിയിൽ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, സ്ഥിരം അക്ഷരം*> || std::is_same_v<std::decay_t<N>, അക്ഷരം*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, പൂർണ്ണസംഖ്യ> = 0

template<typename N>
വരിയിൽ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, സ്ഥിരം അക്ഷരം *> || std::is_same_v<std::decay_t<N>, അക്ഷരം *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, പൂർണ്ണസംഖ്യ> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ തിരികെ _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ തിരികെ _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

ഘടന RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	ശൂന്യം bufNormalize(bool enable=true);

	പൂർണ്ണസംഖ്യ refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

സ്വകാര്യ:
	TwinePool twines_;

പൊതു:
	TwinePool &twines() { തിരികെ twines_; }
	സ്ഥിരം TwinePool &twines() സ്ഥിരം { തിരികെ twines_; }

	ശൂന്യം absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_കണ്ണി>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_അട്ടി;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	സ്ഥിരം RTLIL::Module *module(RTLIL::IdString name) സ്ഥിരം;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ തിരികെ module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> സ്ഥിരം RTLIL::Module *module(N name) സ്ഥിരം
		{ തിരികെ module(twines().find(std::string_view(name))); }
	RTLIL::Module *മുകൾ_module() സ്ഥിരം;

	bool has(RTLIL::IdString id) സ്ഥിരം {
		twines_.check_owned(id);
		തിരികെ modules_.എണ്ണം(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) സ്ഥിരം
		{ തിരികെ has(twines().find(std::string_view(name))); }

	ശൂന്യം add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	ശൂന്യം remove(RTLIL::Module *module);
	ശൂന്യം rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	ശൂന്യം scratchpad_unset(സ്ഥിരം std::string &varname);

	ശൂന്യം scratchpad_set_int(സ്ഥിരം std::string &varname, പൂർണ്ണസംഖ്യ മൂല്യം);
	ശൂന്യം scratchpad_set_bool(സ്ഥിരം std::string &varname, bool മൂല്യം);
	ശൂന്യം scratchpad_set_string(സ്ഥിരം std::string &varname, std::string മൂല്യം);

	പൂർണ്ണസംഖ്യ scratchpad_get_int(സ്ഥിരം std::string &varname, പൂർണ്ണസംഖ്യ default_മൂല്യം_2 = 0) സ്ഥിരം;
	bool scratchpad_get_bool(സ്ഥിരം std::string &varname, bool default_മൂല്യം_2 = false) സ്ഥിരം;
	std::string scratchpad_get_string(സ്ഥിരം std::string &varname, സ്ഥിരം std::string &default_മൂല്യം_2 = std::string()) സ്ഥിരം;

	ശൂന്യം sort();
	ശൂന്യം sort_modules();
	ശൂന്യം check();
	ശൂന്യം optimize();

	ശൂന്യം clone_into(RTLIL::Design *dst) സ്ഥിരം;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) സ്ഥിരം;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) സ്ഥിരം;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) സ്ഥിരം;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) സ്ഥിരം;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) സ്ഥിരം;

	// push the given selection to the selection stack
	ശൂന്യം തള്ളുക_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	ശൂന്യം തള്ളുക_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	ശൂന്യം തള്ളുക_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	ശൂന്യം തള്ളുക_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	ശൂന്യം എടുക്കുക_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		തിരികെ selection_അട്ടി.back();
	}

	// get the current selection
	സ്ഥിരം RTLIL::Selection &selection() സ്ഥിരം {
		തിരികെ selection_അട്ടി.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() സ്ഥിരം {
		തിരികെ selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) സ്ഥിരം {
		തിരികെ selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) സ്ഥിരം {
		തിരികെ selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> ശൂന്യം select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> ശൂന്യം select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) സ്ഥിരം;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() സ്ഥിരം { തിരികെ selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() സ്ഥിരം { തിരികെ selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() സ്ഥിരം { തിരികെ selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() സ്ഥിരം { തിരികെ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() സ്ഥിരം { തിരികെ selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) സ്ഥിരം { തിരികെ selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() സ്ഥിരം { തിരികെ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() സ്ഥിരം { തിരികെ selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Design*> *get_all_designs(ശൂന്യം);

	std::string to_rtlil_str(bool only_selected = true) സ്ഥിരം;
};

ഘടന RTLIL::Module : പൊതു RTLIL::AttrObject
{
	friend ഘടന RTLIL::Cell;
	friend ഘടന RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

സംരക്ഷിത:
	ശൂന്യം add(RTLIL::Wire *wire);
	ശൂന്യം add(RTLIL::Cell *cell);
	ശൂന്യം add(RTLIL::Process *process);

പൊതു:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() സ്ഥിരം { തിരികെ design->twines(); }

	പൂർണ്ണസംഖ്യ refcount_wires_;
	പൂർണ്ണസംഖ്യ refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_മൂല്യങ്ങൾ;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	മായാ ~Module();
	മായാ RTLIL::IdString derive(RTLIL::Design *design, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	മായാ RTLIL::IdString derive(RTLIL::Design *design, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Const> &parameters, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Module*> &interfaces, സ്ഥിരം dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	മായാ size_t എണ്ണം_id(RTLIL::IdString id);
	മായാ ശൂന്യം expand_interfaces(RTLIL::Design *design, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	മായാ bool reprocess_if_necessary(RTLIL::Design *design);

	മായാ ശൂന്യം sort();
	മായാ ശൂന്യം check();
	മായാ ശൂന്യം optimize();
	മായാ ശൂന്യം makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) സ്ഥിരം {
		തിരികെ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ശൂന്യം connect(സ്ഥിരം RTLIL::SigSig &conn);
	ശൂന്യം connect(സ്ഥിരം RTLIL::SigSpec &lhs, സ്ഥിരം RTLIL::SigSpec &rhs);
	ശൂന്യം new_connections(സ്ഥിരം std::vector<RTLIL::SigSig> &new_conn);
	സ്ഥിരം std::vector<RTLIL::SigSig> &connections() സ്ഥിരം;

	std::vector<RTLIL::IdString> ports;
	ശൂന്യം fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	ശൂന്യം bufNormalize();

	template<typename T> ശൂന്യം rewrite_sigspecs(T &functor);
	template<typename T> ശൂന്യം rewrite_sigspecs2(T &functor);
	ശൂന്യം cloneInto(RTLIL::Module *new_mod) സ്ഥിരം;
	മായാ RTLIL::Module *clone() സ്ഥിരം;
	മായാ RTLIL::Module *clone(RTLIL::Design *dst) സ്ഥിരം;
	മായാ RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) സ്ഥിരം;

	bool has_memories() സ്ഥിരം;
	bool has_processes() സ്ഥിരം;

	bool has_memories_warn() സ്ഥിരം;
	bool has_processes_warn() സ്ഥിരം;

	bool is_selected() സ്ഥിരം;
	bool is_selected_whole() സ്ഥിരം;

	std::vector<RTLIL::Wire*> selected_wires() സ്ഥിരം;
	std::vector<RTLIL::Cell*> selected_cells() സ്ഥിരം;
	std::vector<RTLIL::Memory*> selected_memories() സ്ഥിരം;
	std::vector<RTLIL::Process*> selected_processes() സ്ഥിരം;
	std::vector<RTLIL::SelectedMember> selected_members() സ്ഥിരം;

	template<typename T> bool selected(T *member) സ്ഥിരം {
		തിരികെ design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		എങ്കിൽ (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		തിരികെ it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		എങ്കിൽ (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		തിരികെ it == cells_.end() ? nullptr : it->second;
	}

	സ്ഥിരം RTLIL::Wire* wire(RTLIL::IdString id) സ്ഥിരം{
		എങ്കിൽ (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		തിരികെ it == wires_.end() ? nullptr : it->second;
	}
	സ്ഥിരം RTLIL::Cell* cell(RTLIL::IdString id) സ്ഥിരം {
		എങ്കിൽ (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		തിരികെ it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ തിരികെ wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ തിരികെ cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> സ്ഥിരം RTLIL::Wire *wire(N name) സ്ഥിരം
		{ തിരികെ wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> സ്ഥിരം RTLIL::Cell *cell(N name) സ്ഥിരം
		{ തിരികെ cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { തിരികെ RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	പൂർണ്ണസംഖ്യ wires_size() സ്ഥിരം { തിരികെ wires_.size(); }
	RTLIL::Wire* wire_at(പൂർണ്ണസംഖ്യ index) സ്ഥിരം { തിരികെ wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { തിരികെ RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	പൂർണ്ണസംഖ്യ cells_size() സ്ഥിരം { തിരികെ cells_.size(); }
	RTLIL::Cell* cell_at(പൂർണ്ണസംഖ്യ index) സ്ഥിരം { തിരികെ cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	ശൂന്യം remove(സ്ഥിരം pool<RTLIL::Wire*> &wires);
	ശൂന്യം remove(RTLIL::Cell *cell);
	ശൂന്യം remove(RTLIL::Memory *memory);
	ശൂന്യം remove(RTLIL::Process *process);

	ശൂന്യം rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	ശൂന്യം rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	ശൂന്യം rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	ശൂന്യം swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	ശൂന്യം swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, പൂർണ്ണസംഖ്യ &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, സ്ഥിരം RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, സ്ഥിരം RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { തിരികെ addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, സ്ഥിരം RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, സ്ഥിരം RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം RTLIL::SigSpec &sig_x, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, RTLIL::Const offset, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, RTLIL::Const lut, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr, സ്ഥിരം RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_d,   സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en,  സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, RTLIL::Const arst_മൂല്യം, bool clk_polarity = true, bool arst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst,  സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, RTLIL::Const arst_മൂല്യം, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_aload,  സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, RTLIL::Const srst_മൂല്യം, bool clk_polarity = true, bool srst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst,  സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, RTLIL::Const srst_മൂല്യം, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, RTLIL::Const srst_മൂല്യം, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool en_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, RTLIL::Const arst_മൂല്യം, bool en_polarity = true, bool arst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_s, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_s, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം RTLIL::SigBit &sig_d, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം RTLIL::SigBit &sig_d, സ്ഥിരം RTLIL::SigBit &sig_y, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
			സ്ഥിരം RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			bool arst_മൂല്യം = false, bool clk_polarity = true, bool arst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			bool arst_മൂല്യം = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			സ്ഥിരം RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			സ്ഥിരം RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			bool srst_മൂല്യം = false, bool clk_polarity = true, bool srst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			bool srst_മൂല്യം = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			bool srst_മൂല്യം = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool en_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
			bool arst_മൂല്യം = false, bool en_polarity = true, bool arst_polarity = true, സ്ഥിരം std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, സ്ഥിരം std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, bool is_signed = false, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_s, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_s, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം RTLIL::SigBit &sig_d, സ്ഥിരം std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig_a, സ്ഥിരം RTLIL::SigBit &sig_b, സ്ഥിരം RTLIL::SigBit &sig_c, സ്ഥിരം RTLIL::SigBit &sig_d, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, പൂർണ്ണസംഖ്യ width = 1, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, പൂർണ്ണസംഖ്യ width = 1, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, പൂർണ്ണസംഖ്യ width = 1, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, പൂർണ്ണസംഖ്യ width = 1, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, സ്ഥിരം std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_e, സ്ഥിരം std::string &src = "");
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

	std::string to_rtlil_str() സ്ഥിരം;
#ifdef YOSYS_ENABLE_PYTHON
	സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Module*> *get_all_modules(ശൂന്യം);
#endif
};

ഘടന RTLIL::Wire : പൊതു RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

സംരക്ഷിത:
	// use module->addWire() and module->remove() to create or destroy wires
	friend ഘടന RTLIL::Module;
	Wire();
	~Wire();

	friend ഘടന RTLIL::Design;
	friend ഘടന RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

പൊതു:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	ശൂന്യം operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	പൂർണ്ണസംഖ്യ width, start_offset, port_id;
	bool port_നിവേശം, port_output, upto, is_signed;

	RTLIL::Design *design() സ്ഥിരം { തിരികെ module ? module->design : nullptr; }
	TwinePool &twines() സ്ഥിരം { തിരികെ module->twines(); }

	bool known_driver() സ്ഥിരം { തിരികെ driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() സ്ഥിരം    { log_assert(driverCell_); തിരികെ driverCell_; };
	RTLIL::IdString driverPort() സ്ഥിരം { log_assert(driverCell_); തിരികെ driverPort_; };

	പൂർണ്ണസംഖ്യ from_hdl_index(പൂർണ്ണസംഖ്യ hdl_index) {
		പൂർണ്ണസംഖ്യ zero_index = hdl_index - start_offset;
		പൂർണ്ണസംഖ്യ rtlil_index = upto ? width - 1 - zero_index : zero_index;
		തിരികെ rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	പൂർണ്ണസംഖ്യ to_hdl_index(പൂർണ്ണസംഖ്യ rtlil_index) {
		എങ്കിൽ (rtlil_index < 0 || rtlil_index >= width)
			തിരികെ INT_MIN;
		പൂർണ്ണസംഖ്യ zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		തിരികെ zero_index + start_offset;
	}

	std::string to_rtlil_str() സ്ഥിരം;

#ifdef YOSYS_ENABLE_PYTHON
	സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Wire*> *get_all_wires(ശൂന്യം);
#endif
};

വരിയിൽ പൂർണ്ണസംഖ്യ GetSize(RTLIL::Wire *wire) {
	തിരികെ wire->width;
}

ഘടന RTLIL::Memory : പൊതു RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() സ്ഥിരം { തിരികെ module ? module->design : nullptr; }
	TwinePool &twines() സ്ഥിരം { തിരികെ module->twines(); }

	RTLIL::MemoryNameId name;

	പൂർണ്ണസംഖ്യ width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Memory*> *get_all_memorys(ശൂന്യം);
#endif

	std::string to_rtlil_str() സ്ഥിരം;
};

ഘടന RTLIL::Cell : പൊതു RTLIL::AttrObject
{
സ്വകാര്യ:
	ശൂന്യം initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &സൂചന, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
പൊതു:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

സംരക്ഷിത:
	// use module->addCell() and module->remove() to create or destroy cells
	friend ഘടന RTLIL::Module;
	Cell();
	~Cell();

പൊതു:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	ശൂന്യം operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() സ്ഥിരം { തിരികെ module ? module->design : nullptr; }
	TwinePool &twines() സ്ഥിരം { തിരികെ module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) സ്ഥിരം;
	ശൂന്യം unsetPort(RTLIL::IdString portname);
	ശൂന്യം setPort(RTLIL::IdString portname, RTLIL::SigSpec സൂചന);
	template<typename N, YS_NAME_STRING(N)> ശൂന്യം setPort(N portname, RTLIL::SigSpec സൂചന)
		{ setPort(module->twines().add(std::move(portname)), std::move(സൂചന)); }
	സ്ഥിരം RTLIL::SigSpec &getPort(RTLIL::IdString portname) സ്ഥിരം;
	സ്ഥിരം dict<RTLIL::IdString, RTLIL::SigSpec> &connections() സ്ഥിരം;

	bool known() സ്ഥിരം;
	bool നിവേശം(RTLIL::IdString portname) സ്ഥിരം;
	bool output(RTLIL::IdString portname) സ്ഥിരം;
	PortDir port_dir(RTLIL::IdString portname) സ്ഥിരം;

	bool hasParam(RTLIL::IdString paramname) സ്ഥിരം;
	ശൂന്യം unsetParam(RTLIL::IdString paramname);
	ശൂന്യം setParam(RTLIL::IdString paramname, RTLIL::Const മൂല്യം);
	സ്ഥിരം RTLIL::Const &getParam(RTLIL::IdString paramname) സ്ഥിരം;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) സ്ഥിരം
		{ തിരികെ hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ശൂന്യം unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ശൂന്യം setParam(N name, RTLIL::Const മൂല്യം)
		{ setParam(module->twines().add(std::move(name)), std::move(മൂല്യം)); }
	template<typename N, YS_NAME_STRING(N)> സ്ഥിരം RTLIL::Const &getParam(N name) സ്ഥിരം
		{ തിരികെ getParam(module->twines().find(std::string_view(name))); }

	ശൂന്യം sort();
	ശൂന്യം check();
	ശൂന്യം fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() സ്ഥിരം;

	template<typename T> ശൂന്യം rewrite_sigspecs(T &functor);
	template<typename T> ശൂന്യം rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Cell*> *get_all_cells(ശൂന്യം);
#endif

	bool has_memid() സ്ഥിരം;
	bool is_mem_cell() സ്ഥിരം;
	bool is_builtin_ff() സ്ഥിരം;

	std::string to_rtlil_str() സ്ഥിരം;
};

ഘടന RTLIL::CaseRule : പൊതു RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() സ്ഥിരം;

	template<typename T> ശൂന്യം rewrite_sigspecs(T &functor);
	template<typename T> ശൂന്യം rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() സ്ഥിരം;
};

ഘടന RTLIL::SwitchRule : പൊതു RTLIL::AttrObject
{
	RTLIL::SigSpec സൂചന;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() സ്ഥിരം;

	template<typename T> ശൂന്യം rewrite_sigspecs(T &functor);
	template<typename T> ശൂന്യം rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() സ്ഥിരം;
};

ഘടന RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

ഘടന RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec സൂചന;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> ശൂന്യം rewrite_sigspecs(T &functor);
	template<typename T> ശൂന്യം rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() സ്ഥിരം;
};

ഘടന RTLIL::Process : പൊതു RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(hashidx_); തിരികെ h; }

സംരക്ഷിത:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend ഘടന RTLIL::Module;
	Process();
	~Process();

പൊതു:
	RTLIL::Module *module;
	RTLIL::CaseRule വേര്_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() സ്ഥിരം { തിരികെ module ? module->design : nullptr; }
	TwinePool &twines() സ്ഥിരം { തിരികെ module->twines(); }

	template<typename T> ശൂന്യം rewrite_sigspecs(T &functor);
	template<typename T> ശൂന്യം rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() സ്ഥിരം;

	std::string to_rtlil_str() സ്ഥിരം;
};

വരിയിൽ RTLIL::SigBit::SigBit() : wire(ശൂന്യ), data(RTLIL::State::S0) { }
വരിയിൽ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(ശൂന്യ), data(bit) { }
വരിയിൽ RTLIL::SigBit::SigBit(bool bit) : wire(ശൂന്യ), data(bit ? State::S1 : State::S0) { }
വരിയിൽ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
വരിയിൽ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, പൂർണ്ണസംഖ്യ offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
വരിയിൽ RTLIL::SigBit::SigBit(സ്ഥിരം RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); എങ്കിൽ (wire) offset = chunk.offset; അല്ലെങ്കിൽ data = chunk.data[0]; }
വരിയിൽ RTLIL::SigBit::SigBit(സ്ഥിരം RTLIL::SigChunk &chunk, പൂർണ്ണസംഖ്യ index) : wire(chunk.wire) { എങ്കിൽ (wire) offset = chunk.offset + index; അല്ലെങ്കിൽ data = chunk.data[index]; }

വരിയിൽ bool RTLIL::SigBit::operator<(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം {
	എങ്കിൽ (wire == other.wire)
		തിരികെ wire ? (offset < other.offset) : (data < other.data);
	എങ്കിൽ (wire != nullptr && other.wire != nullptr)
		തിരികെ wire->name < other.wire->name;
	തിരികെ (wire != nullptr) < (other.wire != nullptr);
}

വരിയിൽ bool RTLIL::SigBit::operator==(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം {
	തിരികെ (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

വരിയിൽ bool RTLIL::SigBit::operator!=(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം {
	തിരികെ (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

വരിയിൽ Hasher RTLIL::SigBit::hash_into(Hasher h) സ്ഥിരം {
	എങ്കിൽ (wire) {
		h.eat(offset);
		h.eat(wire->name);
		തിരികെ h;
	}
	h.eat(data);
	തിരികെ h;
}


വരിയിൽ Hasher RTLIL::SigBit::hash_മുകൾ() സ്ഥിരം {
	Hasher h;
	എങ്കിൽ (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		തിരികെ h;
	}
	h.force(data);
	തിരികെ h;
}

വരിയിൽ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() സ്ഥിരം {
	തിരികെ (*sig_p)[index];
}

വരിയിൽ സ്ഥിരം RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	തിരികെ bit;
}

വരിയിൽ RTLIL::SigBit::SigBit(സ്ഥിരം RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
ശൂന്യം RTLIL::Module::rewrite_sigspecs(T &functor)
{
	വേണ്ടി (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	വേണ്ടി (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	വേണ്ടി (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
ശൂന്യം RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	വേണ്ടി (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	വേണ്ടി (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	വേണ്ടി (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
ശൂന്യം RTLIL::Cell::rewrite_sigspecs(T &functor) {
	വേണ്ടി (auto &it : connections_)
		functor(it.second);
}

template<typename T>
ശൂന്യം RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	വേണ്ടി (auto &it : connections_)
		functor(it.second);
}

template<typename T>
ശൂന്യം RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	വേണ്ടി (auto &it : compare)
		functor(it);
	വേണ്ടി (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	വേണ്ടി (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ശൂന്യം RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	വേണ്ടി (auto &it : compare)
		functor(it);
	വേണ്ടി (auto &it : actions) {
		functor(it.first, it.second);
	}
	വേണ്ടി (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
ശൂന്യം RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(സൂചന);
	വേണ്ടി (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ശൂന്യം RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(സൂചന);
	വേണ്ടി (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
ശൂന്യം RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(സൂചന);
	വേണ്ടി (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	വേണ്ടി (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
ശൂന്യം RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(സൂചന);
	വേണ്ടി (auto &it : actions) {
		functor(it.first, it.second);
	}
	വേണ്ടി (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
ശൂന്യം RTLIL::Process::rewrite_sigspecs(T &functor)
{
	വേര്_case.rewrite_sigspecs(functor);
	വേണ്ടി (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ശൂന്യം RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	വേര്_case.rewrite_sigspecs2(functor);
	വേണ്ടി (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#അന്തര്ഭാവ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
