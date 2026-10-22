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

#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/twine.h"

#அந்தர்பாவ <deque>
#அந்தர்பாவ <string_view>
#அந்தர்பாவ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	எண்ணிக்கை State : unsigned எழுத்து {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	எண்ணிக்கை SyncType : unsigned எழுத்து {
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
	எண்ணிக்கை ConstFlags : unsigned எழுத்து {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	எண்ணிக்கை SelectPartials : unsigned எழுத்து {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	எண்ணிக்கை SelectBoxes : unsigned எழுத்து {
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

	எண்ணிக்கை PortDir : unsigned எழுத்து  {
		PD_UNKNOWN = 0,
		PD_உள்ளீடு = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr முழு WIDTH_LIMIT = 1 << 30;

	கட்டமைப்பு Const;
	கட்டமைப்பு AttrObject;
	கட்டமைப்பு SelectedMember;
	கட்டமைப்பு Selection;
	கட்டமைப்பு Monitor;
	கட்டமைப்பு Design;
	கட்டமைப்பு Module;
	கட்டமைப்பு Wire;
	கட்டமைப்பு Memory;
	கட்டமைப்பு Cell;
	கட்டமைப்பு SigChunk;
	கட்டமைப்பு SigBit;
	கட்டமைப்பு SigSpecIterator;
	கட்டமைப்பு SigSpecConstIterator;
	கட்டமைப்பு SigSpec;
	கட்டமைப்பு CaseRule;
	கட்டமைப்பு SwitchRule;
	கட்டமைப்பு MemWriteAction;
	கட்டமைப்பு SyncRule;
	கட்டமைப்பு Process;

	வகைவரை std::pair<SigSpec, SigSpec> SigSig;
};

கட்டமைப்பு SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	வெறுமை copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			மாறிலி dict<IdString, RTLIL::Const> &src,
			மாறிலி RTLIL::Design *src_design, RTLIL::Design *dst_design);

	வெளி dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	மாறிலி pool<IdString> &builtin_ff_cell_types();

	நிலையான உள்வரி std::string escape_id(மாறிலி std::string &str) {
		எனில் (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			திரும்பு "\\" + str;
		திரும்பு str;
	}

	நிலையான உள்வரி std::string unescape_id(மாறிலி std::string &str) {
		எனில் (str.size() < 2)
			திரும்பு str;
		எனில் (str[0] != '\\')
			திரும்பு str;
		எனில் (str[1] == '$' || str[1] == '\\')
			திரும்பு str;
		எனில் (str[1] >= '0' && str[1] <= '9')
			திரும்பு str;
		திரும்பு str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> கட்டமைப்பு sort_by_name_str {
		bool operator()(T *a, T *b) மாறிலி {
			திரும்பு a->name.lt_by_name(b->name);
		}
	};

	கட்டமைப்பு sort_by_id_str {
		மாறிலி TwinePool& pool;
		explicit sort_by_id_str(மாறிலி TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) மாறிலி {
			திரும்பு pool.compare_by_name(a, b) < 0;
		}
	};

	நிலையான உள்வரி std::string encode_filename(மாறிலி std::string &filename)
	{
		std::stringstream val;
		எனில் (!std::any_of(filename.begin(), filename.end(), [](எழுத்து c) {
			திரும்பு static_cast<unsigned எழுத்து>(c) < 33 || static_cast<unsigned எழுத்து>(c) > 126;
		})) திரும்பு filename;
		க்கு (unsigned எழுத்து மாறிலி c : filename) {
			எனில் (c < 33 || c > 126)
				val << stringf("$%02x", c);
			இல்லையெனில்
				val << c;
		}
		திரும்பு val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_and         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_or          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_xor         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_xnor        (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_reduce_and  (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_reduce_or   (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_reduce_xor  (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_reduce_xnor (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_reduce_bool (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_logic_not   (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_logic_and   (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_logic_or    (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_shl         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_shr         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_sshl        (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_sshr        (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_shift       (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_shiftx      (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_lt          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_le          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_eq          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_ne          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_eqx         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_nex         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_ge          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_gt          (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_add         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_sub         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_mul         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_div         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_divfloor    (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_modfloor    (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_mod         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_pow         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_pos         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_buf         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);
	RTLIL::Const const_neg         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, bool signed1, bool signed2, முழு result_len);

	RTLIL::Const const_mux         (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, மாறிலி RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, மாறிலி RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2);
	RTLIL::Const const_demux       (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, மாறிலி RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename சாவி_2 = IdString>
	கட்டமைப்பு ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using மதிப்பு_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<சாவி_2, T>::iterator it;
		dict<சாவி_2, T> *list_p;
		முழு *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, முழு *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			எனில் (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} இல்லையெனில் {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(மாறிலி RTLIL::ObjIterator<T, சாவி_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			எனில் (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(மாறிலி RTLIL::ObjIterator<T, சாவி_2> &other) {
			எனில் (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			எனில் (refcount_p)
				(*refcount_p)++;
			திரும்பு *this;
		}

		~ObjIterator() {
			எனில் (refcount_p)
				(*refcount_p)--;
		}

		உள்வரி T operator*() மாறிலி {
			log_assert(list_p != nullptr);
			திரும்பு it->second;
		}

		உள்வரி bool operator!=(மாறிலி RTLIL::ObjIterator<T, சாவி_2> &other) மாறிலி {
			எனில் (list_p == nullptr || other.list_p == nullptr)
				திரும்பு list_p != other.list_p;
			திரும்பு it != other.it;
		}


		உள்வரி bool operator==(மாறிலி RTLIL::ObjIterator<T, சாவி_2> &other) மாறிலி {
			திரும்பு !(*this != other);
		}

		உள்வரி ObjIterator<T, சாவி_2>& operator++() {
			log_assert(list_p != nullptr);
			எனில் (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			திரும்பு *this;
		}

		உள்வரி ObjIterator<T, சாவி_2>& operator+=(முழு amt) {
			log_assert(list_p != nullptr);
			it += amt;
			எனில் (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			திரும்பு *this;
		}

		உள்வரி ObjIterator<T, சாவி_2> operator+(முழு amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, சாவி_2> new_obj(*this);
			new_obj.it += amt;
			எனில் (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			திரும்பு new_obj;
		}

		உள்வரி மாறிலி ObjIterator<T, சாவி_2> operator++(முழு) {
			ObjIterator<T, சாவி_2> result(*this);
			++(*this);
			திரும்பு result;
		}
	};

	template<typename T, typename சாவி_2 = IdString>
	கட்டமைப்பு ObjRange
	{
		dict<சாவி_2, T> *list_p;
		முழு *refcount_p;

		ObjRange(decltype(list_p) list_p, முழு *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, சாவி_2> begin() { திரும்பு RTLIL::ObjIterator<T, சாவி_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, சாவி_2> end() { திரும்பு RTLIL::ObjIterator<T, சாவி_2>(); }

		size_t size() மாறிலி {
			திரும்பு list_p->size();
		}

		operator pool<T>() மாறிலி {
			pool<T> result;
			க்கு (auto &it : *list_p)
				result.செருகு(it.second);
			திரும்பு result;
		}

		operator std::vector<T>() மாறிலி {
			std::vector<T> result;
			result.reserve(list_p->size());
			க்கு (auto &it : *list_p)
				result.தள்ளு_back(it.second);
			திரும்பு result;
		}

		pool<T> to_pool() மாறிலி { திரும்பு *this; }
		std::vector<T> to_vector() மாறிலி { திரும்பு *this; }
	};
};

கட்டமைப்பு RTLIL::Const
{
	short முழு flags;
தனிப்பட்ட:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	எண்ணிக்கை class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	ஒன்றிப்பு {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() மாறிலி { திரும்பு tag == backing_tag::bits; }
	bool is_str() மாறிலி { திரும்பு tag == backing_tag::string; }

	bitvectype* get_if_bits() { திரும்பு is_bits() ? &bits_ : வெற்று; }
	std::string* get_if_str() { திரும்பு is_str() ? &str_ : வெற்று; }
	மாறிலி bitvectype* get_if_bits() மாறிலி { திரும்பு is_bits() ? &bits_ : வெற்று; }
	மாறிலி std::string* get_if_str() மாறிலி { திரும்பு is_str() ? &str_ : வெற்று; }

	bitvectype& get_bits();
	std::string& get_str();
	மாறிலி bitvectype& get_bits() மாறிலி;
	மாறிலி std::string& get_str() மாறிலி;
	std::vector<RTLIL::State>& bits_internal();
	வெறுமை bitvectorize_internal();

பொது:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, முழு width);
	Const(RTLIL::State bit, முழு width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(மாறிலி std::vector<bool> &bits);
	Const(மாறிலி RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(மாறிலி RTLIL::Const &other);
	~Const();

	கட்டமைப்பு Builder
	{
		Builder() {}
		Builder(முழு expected_width) { bits.reserve(expected_width); }
		வெறுமை தள்ளு_back(RTLIL::State b) { bits.தள்ளு_back(b); }
		முழு size() மாறிலி { திரும்பு static_cast<முழு>(bits.size()); }
		Const build() { திரும்பு Const(std::move(bits)); }
	தனிப்பட்ட:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(மாறிலி RTLIL::Const &other) மாறிலி;
	bool operator ==(மாறிலி RTLIL::Const &other) மாறிலி;
	bool operator !=(மாறிலி RTLIL::Const &other) மாறிலி;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { திரும்பு bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	வெறுமை bitvectorize() மாறிலி { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() மாறிலி;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	முழு as_int(bool is_signed = false) மாறிலி;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) மாறிலி;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<முழு> try_as_int(bool is_signed = false) மாறிலி;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	முழு as_int_saturating(bool is_signed = false) மாறிலி;

	வெறுமை tag_bare_integer_const(மாறிலி std::string &மதிப்பு);

	std::string as_string(மாறிலி எழுத்து* any = "-") மாறிலி;
	நிலையான Const from_string(மாறிலி std::string &str);
	std::vector<RTLIL::State> to_bits() மாறிலி;

	std::string decode_string() மாறிலி;
	முழு size() மாறிலி;
	bool empty() மாறிலி;

	வெறுமை append(மாறிலி RTLIL::Const &other);
	வெறுமை set(முழு i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	வெறுமை resize(முழு size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	தனிப்பட்ட:
		மாறிலி Const* parent;
		size_t idx;

	பொது:
		using iterator_category = std::bidirectional_iterator_tag;
		using மதிப்பு_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = மாறிலி State*;
		using reference = மாறிலி State&;

		const_iterator(மாறிலி Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() மாறிலி;

		const_iterator& operator++() { ++idx; திரும்பு *this; }
		const_iterator& operator--() { --idx; திரும்பு *this; }
		const_iterator operator++(முழு) { const_iterator result(*this); ++idx; திரும்பு result; }
		const_iterator operator--(முழு) { const_iterator result(*this); --idx; திரும்பு result; }
		const_iterator& operator+=(முழு i) { idx += i; திரும்பு *this; }

		const_iterator operator+(முழு add) {
			திரும்பு const_iterator(*parent, idx + add);
		}
		const_iterator operator-(முழு sub) {
			திரும்பு const_iterator(*parent, idx - sub);
		}
		முழு operator-(மாறிலி const_iterator& other) {
			திரும்பு idx - other.idx;
		}

		bool operator==(மாறிலி const_iterator& other) மாறிலி {
			திரும்பு idx == other.idx;
		}

		bool operator!=(மாறிலி const_iterator& other) மாறிலி {
			திரும்பு !(*this == other);
		}
	};

	class iterator {
	தனிப்பட்ட:
		Const* parent;
		size_t idx;

	பொது:
		class proxy {
		தனிப்பட்ட:
			Const* parent;
			size_t idx;
		பொது:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() மாறிலி { திரும்பு (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); திரும்பு *this; }
			proxy& operator=(மாறிலி proxy& other) { parent->set(idx, (*other.parent)[other.idx]); திரும்பு *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using மதிப்பு_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() மாறிலி { திரும்பு proxy(parent, idx); }
		iterator& operator++() { ++idx; திரும்பு *this; }
		iterator& operator--() { --idx; திரும்பு *this; }
		iterator operator++(முழு) { iterator result(*this); ++idx; திரும்பு result; }
		iterator operator--(முழு) { iterator result(*this); --idx; திரும்பு result; }
		iterator& operator+=(முழு i) { idx += i; திரும்பு *this; }

		iterator operator+(முழு add) {
			திரும்பு iterator(*parent, idx + add);
		}
		iterator operator-(முழு sub) {
			திரும்பு iterator(*parent, idx - sub);
		}
		முழு operator-(மாறிலி iterator& other) {
			திரும்பு idx - other.idx;
		}

		bool operator==(மாறிலி iterator& other) மாறிலி {
			திரும்பு idx == other.idx;
		}

		bool operator!=(மாறிலி iterator& other) மாறிலி {
			திரும்பு !(*this == other);
		}
	};

	const_iterator begin() மாறிலி {
		திரும்பு const_iterator(*this, 0);
	}
	const_iterator end() மாறிலி {
		திரும்பு const_iterator(*this, size());
	}
	iterator begin() {
		திரும்பு iterator(*this, 0);
	}
	iterator end() {
		திரும்பு iterator(*this, size());
	}
	State back() மாறிலி {
		திரும்பு *(end() - 1);
	}
	State front() மாறிலி {
		திரும்பு *begin();
	}
	State at(size_t i) மாறிலி {
		திரும்பு *const_iterator(*this, i);
	}
	State operator[](size_t i) மாறிலி {
		திரும்பு *const_iterator(*this, i);
	}

	bool is_fully_zero() மாறிலி;
	bool is_fully_ones() மாறிலி;
	bool is_fully_def() மாறிலி;
	bool is_fully_undef() மாறிலி;
	bool is_fully_undef_x_only() மாறிலி;
	bool is_onehot(முழு *pos = nullptr) மாறிலி;

	RTLIL::Const extract(முழு offset, முழு len = 1, RTLIL::State padding = RTLIL::State::S0) மாறிலி;

	// find the MSB without redundant leading bits
	முழு get_min_size(bool is_signed) மாறிலி;

	// compress representation to the minimum required bits
	வெறுமை compress(bool is_signed = false);

	std::optional<முழு> as_int_compress(bool is_signed) மாறிலி;

	வெறுமை extu(முழு width) {
		resize(width, RTLIL::State::S0);
	}

	வெறுமை exts(முழு width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
};

கட்டமைப்பு RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) மாறிலி;

	வெறுமை set_bool_attribute(RTLIL::IdString id, bool மதிப்பு=true);
	bool get_bool_attribute(RTLIL::IdString id) மாறிலி;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) மாறிலி {
		திரும்பு get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	வெறுமை set_string_attribute(RTLIL::IdString id, string மதிப்பு);
	string get_string_attribute(RTLIL::IdString id) மாறிலி;

	வெறுமை set_strpool_attribute(RTLIL::IdString id, மாறிலி pool<string> &data);
	வெறுமை add_strpool_attribute(RTLIL::IdString id, மாறிலி pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) மாறிலி;

	வெறுமை set_src_attribute(மாறிலி std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() மாறிலி {
		திரும்பு get_string_attribute(ID::src);
	}

	வெறுமை set_hdlname_attribute(மாறிலி vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() மாறிலி;

	வெறுமை set_intvec_attribute(RTLIL::IdString id, மாறிலி vector<முழு> &data);
	vector<முழு> get_intvec_attribute(RTLIL::IdString id) மாறிலி;
};

#அந்தர்பாவ "kernel/rtlil_twine_compat.h"

கட்டமைப்பு RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

கட்டமைப்பு RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	முழு width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(மாறிலி RTLIL::Const &மதிப்பு) : wire(nullptr), data(மதிப்பு.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&மதிப்பு) : wire(nullptr), data(மதிப்பு.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, முழு offset, முழு width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(மாறிலி std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(முழு val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(முழு val, முழு width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, முழு width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(மாறிலி RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(முழு offset, முழு length) மாறிலி;
	RTLIL::SigBit operator[](முழு offset) மாறிலி;
	உள்வரி முழு size() மாறிலி { திரும்பு width; }
	உள்வரி bool is_wire() மாறிலி { திரும்பு wire != வெற்று; }

	bool operator <(மாறிலி RTLIL::SigChunk &other) மாறிலி;
	bool operator ==(மாறிலி RTLIL::SigChunk &other) மாறிலி;
	bool operator !=(மாறிலி RTLIL::SigChunk &other) மாறிலி;
};

கட்டமைப்பு RTLIL::SigBit
{
	RTLIL::Wire *wire;
	ஒன்றிப்பு {
		RTLIL::State data; // used if wire == NULL
		முழு offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, முழு offset);
	SigBit(மாறிலி RTLIL::SigChunk &chunk);
	SigBit(மாறிலி RTLIL::SigChunk &chunk, முழு index);
	SigBit(மாறிலி RTLIL::SigSpec &sig);
	SigBit(மாறிலி RTLIL::SigBit &sigbit) = இயல்பு;
	RTLIL::SigBit &operator =(மாறிலி RTLIL::SigBit &other) = இயல்பு;

	உள்வரி bool is_wire() மாறிலி { திரும்பு wire != வெற்று; }

	bool operator <(மாறிலி RTLIL::SigBit &other) மாறிலி;
	bool operator ==(மாறிலி RTLIL::SigBit &other) மாறிலி;
	bool operator !=(மாறிலி RTLIL::SigBit &other) மாறிலி;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
	[[nodiscard]] Hasher hash_உச்சி() மாறிலி;
};

namespace hashlib {
	template <>
	கட்டமைப்பு hash_ops<RTLIL::SigBit> {
		நிலையான உள்வரி bool cmp(மாறிலி RTLIL::SigBit &a, மாறிலி RTLIL::SigBit &b) {
			திரும்பு a == b;
		}
		[[nodiscard]] நிலையான உள்வரி Hasher hash(மாறிலி RTLIL::SigBit sb) {
			திரும்பு sb.hash_உச்சி();
		}
		[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி RTLIL::SigBit sb, Hasher h) {
			திரும்பு sb.hash_into(h);
		}
	};
};

கட்டமைப்பு RTLIL::SigSpecIterator
{
	வகைவரை std::உள்ளீடு_iterator_tag iterator_category;
	வகைவரை RTLIL::SigBit மதிப்பு_type;
	வகைவரை ptrdiff_t difference_type;
	வகைவரை RTLIL::SigBit* pointer;
	வகைவரை RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	முழு index;

	உள்வரி RTLIL::SigBit &operator*() மாறிலி;
	உள்வரி bool operator!=(மாறிலி RTLIL::SigSpecIterator &other) மாறிலி { திரும்பு index != other.index; }
	உள்வரி bool operator==(மாறிலி RTLIL::SigSpecIterator &other) மாறிலி { திரும்பு index == other.index; }
	உள்வரி வெறுமை operator++() { index++; }
};

கட்டமைப்பு RTLIL::SigSpecConstIterator
{
	வகைவரை std::உள்ளீடு_iterator_tag iterator_category;
	வகைவரை RTLIL::SigBit மதிப்பு_type;
	வகைவரை ptrdiff_t difference_type;
	வகைவரை RTLIL::SigBit* pointer;
	வகைவரை RTLIL::SigBit& reference;

	மாறிலி RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	முழு index;

	உள்வரி மாறிலி RTLIL::SigBit &operator*();
	உள்வரி bool operator!=(மாறிலி RTLIL::SigSpecConstIterator &other) மாறிலி { திரும்பு index != other.index; }
	உள்வரி bool operator==(மாறிலி RTLIL::SigSpecIterator &other) மாறிலி { திரும்பு index == other.index; }
	உள்வரி வெறுமை operator++() { index++; }
};

கட்டமைப்பு RTLIL::SigSpec
{
தனிப்பட்ட:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	எண்ணிக்கை Representation : எழுத்து {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	கட்டமைப்பு AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(மாறிலி AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(மாறிலி AtomicHash &rhs) { store(rhs.load()); திரும்பு *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> பட() மாறிலி {
			Hasher::hash_t மதிப்பு = load();
			எனில் (மதிப்பு == 0)
				திரும்பு std::nullopt;
			திரும்பு மதிப்பு;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		வெறுமை set(Hasher::hash_t மதிப்பு) மாறிலி {
			log_assert(மதிப்பு != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(மதிப்பு, std::memory_order_relaxed);
			log_assert(old == 0 || old == மதிப்பு);
		}
		வெறுமை clear() { store(0); }
	தனிப்பட்ட:
		முழு load() மாறிலி { திரும்பு atomic_.load(std::memory_order_relaxed); }
		வெறுமை store(Hasher::hash_t மதிப்பு) மாறிலி {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(மதிப்பு, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	ஒன்றிப்பு {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	வெறுமை init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	வெறுமை unpack();
	உள்வரி வெறுமை inline_unpack() {
		எனில் (rep_ == CHUNK)
			unpack();
	}
	வெறுமை try_repack();

	Hasher::hash_t updhash() மாறிலி;
	வெறுமை destroy() {
		எனில் (rep_ == CHUNK)
			chunk_.~SigChunk();
		இல்லையெனில்
			bits_.~vector();
	}
	friend கட்டமைப்பு Chunks;

பொது:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(மாறிலி SigSpec &மதிப்பு) : rep_(மதிப்பு.rep_), hash_(மதிப்பு.hash_) {
		எனில் (மதிப்பு.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(மதிப்பு.chunk_);
		இல்லையெனில்
			new (&bits_) std::vector<RTLIL::SigBit>(மதிப்பு.bits_);
	}
	SigSpec(SigSpec &&மதிப்பு) : rep_(மதிப்பு.rep_), hash_(மதிப்பு.hash_) {
		எனில் (மதிப்பு.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(மதிப்பு.chunk_));
		இல்லையெனில்
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(மதிப்பு.bits_));
	}
	SigSpec(மாறிலி RTLIL::Const &மதிப்பு);
	SigSpec(RTLIL::Const &&மதிப்பு);
	SigSpec(மாறிலி RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, முழு offset, முழு width = 1);
	SigSpec(மாறிலி std::string &str);
	SigSpec(முழு val, முழு width = 32);
	SigSpec(RTLIL::State bit, முழு width = 1);
	SigSpec(மாறிலி RTLIL::SigBit &bit, முழு width = 1);
	SigSpec(மாறிலி std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(மாறிலி std::vector<RTLIL::SigBit> &bits);
	SigSpec(மாறிலி pool<RTLIL::SigBit> &bits);
	SigSpec(மாறிலி std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(மாறிலி SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		எனில் (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		இல்லையெனில்
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		திரும்பு *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		எனில் (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		இல்லையெனில்
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		திரும்பு *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	கட்டமைப்பு Chunks {
		Chunks(மாறிலி SigSpec &spec) : spec(spec) {}
		கட்டமைப்பு const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using மதிப்பு_type = மாறிலி SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = மாறிலி SigChunk *;
			using reference = மாறிலி SigChunk &;

			மாறிலி SigSpec &spec;
			முழு bit_index;
			SigChunk chunk;

			const_iterator(மாறிலி SigSpec &spec) : spec(spec) {
				bit_index = 0;
				எனில் (spec.rep_ == BITS)
					next_chunk_bits();
			}
			எண்ணிக்கை End { END };
			const_iterator(மாறிலி SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			வெறுமை next_chunk_bits();

			மாறிலி SigChunk &operator*() {
				எனில் (spec.rep_ == CHUNK)
					திரும்பு spec.chunk_;
				திரும்பு chunk;
			};
			மாறிலி SigChunk *operator->() { திரும்பு &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				எனில் (spec.rep_ == BITS)
					next_chunk_bits();
				திரும்பு *this;
			}
			bool operator==(மாறிலி const_iterator &rhs) மாறிலி { திரும்பு bit_index == rhs.bit_index; }
			bool operator!=(மாறிலி const_iterator &rhs) மாறிலி { திரும்பு !(*this == rhs); }
		};
		const_iterator begin() மாறிலி { திரும்பு const_iterator(spec); }
		const_iterator end() மாறிலி {
			const_iterator it(spec, const_iterator::END);
			திரும்பு it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			திரும்பு chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			திரும்பு chunk_vector.rend();
		}
		முழு size() {
			ensure_chunk_vector();
			திரும்பு chunk_vector.size();
		}
		முழு size() மாறிலி {
			திரும்பு std::distance(begin(), end());
		}
		மாறிலி SigChunk &at(முழு index) {
			ensure_chunk_vector();
			திரும்பு chunk_vector.at(index);
		}
		operator மாறிலி std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			திரும்பு chunk_vector;
		}
	தனிப்பட்ட:
		வெறுமை ensure_chunk_vector() {
			எனில் (spec.size() > 0 && chunk_vector.empty()) {
				க்கு (மாறிலி RTLIL::SigChunk &c : *this)
					chunk_vector.தள்ளு_back(c);
			}
		}
		மாறிலி SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend கட்டமைப்பு Chunks::const_iterator;

	உள்வரி Chunks chunks() மாறிலி { திரும்பு {*this}; }
	உள்வரி மாறிலி SigSpec &bits() மாறிலி { திரும்பு *this; }

	உள்வரி முழு size() மாறிலி { திரும்பு rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	உள்வரி bool empty() மாறிலி { திரும்பு size() == 0; };

	உள்வரி RTLIL::SigBit &operator[](முழு index) { inline_unpack(); hash_.clear(); திரும்பு bits_.at(index); }
	உள்வரி RTLIL::SigBit operator[](முழு index) மாறிலி {
		எனில் (rep_ == CHUNK) {
			எனில் (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			எனில் (chunk_.wire)
				திரும்பு RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			திரும்பு RTLIL::SigBit(chunk_.data[index]);
		}
		திரும்பு bits_.at(index);
	}

	உள்வரி RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; திரும்பு it; }
	உள்வரி RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); திரும்பு it; }

	உள்வரி RTLIL::SigSpecConstIterator begin() மாறிலி { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; திரும்பு it; }
	உள்வரி RTLIL::SigSpecConstIterator end() மாறிலி { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); திரும்பு it; }

	வெறுமை sort();
	வெறுமை sort_and_unify();

	வெறுமை replace(மாறிலி RTLIL::SigSpec &pattern, மாறிலி RTLIL::SigSpec &with);
	வெறுமை replace(மாறிலி RTLIL::SigSpec &pattern, மாறிலி RTLIL::SigSpec &with, RTLIL::SigSpec *other) மாறிலி;

	வெறுமை replace(மாறிலி dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	வெறுமை replace(மாறிலி dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) மாறிலி;

	வெறுமை replace(மாறிலி std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	வெறுமை replace(மாறிலி std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) மாறிலி;

	வெறுமை replace(முழு offset, மாறிலி RTLIL::SigSpec &with);

	வெறுமை remove(மாறிலி RTLIL::SigSpec &pattern);
	வெறுமை remove(மாறிலி RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) மாறிலி;
	வெறுமை remove2(மாறிலி RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	வெறுமை remove(மாறிலி pool<RTLIL::SigBit> &pattern);
	வெறுமை remove(மாறிலி pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) மாறிலி;
	வெறுமை remove2(மாறிலி pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	வெறுமை remove2(மாறிலி std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	வெறுமை remove2(மாறிலி pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	வெறுமை remove(முழு offset, முழு length = 1);
	வெறுமை remove_const();

	RTLIL::SigSpec extract(மாறிலி RTLIL::SigSpec &pattern, மாறிலி RTLIL::SigSpec *other = வெற்று) மாறிலி;
	RTLIL::SigSpec extract(மாறிலி pool<RTLIL::SigBit> &pattern, மாறிலி RTLIL::SigSpec *other = வெற்று) மாறிலி;
	RTLIL::SigSpec extract(முழு offset, முழு length = 1) மாறிலி;
	RTLIL::SigSpec extract_end(முழு offset) மாறிலி { திரும்பு extract(offset, size() - offset); }

	வெறுமை rewrite_wires(std::function<வெறுமை(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() மாறிலி { log_assert(size()); திரும்பு (*this)[0]; };
	RTLIL::SigBit msb() மாறிலி { log_assert(size()); திரும்பு (*this)[size() - 1]; };
	RTLIL::SigBit front() மாறிலி { திரும்பு (*this)[0]; }
	RTLIL::SigBit back() மாறிலி { திரும்பு (*this)[size() - 1]; }

	வெறுமை append(மாறிலி RTLIL::SigSpec &சமிக்ஞை);
	உள்வரி வெறுமை append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	உள்வரி வெறுமை append(மாறிலி RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	உள்வரி வெறுமை append(மாறிலி RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	வெறுமை append(மாறிலி RTLIL::SigBit &bit);
	உள்வரி வெறுமை append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	உள்வரி வெறுமை append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	வெறுமை extend_u0(முழு width, bool is_signed = false);

	RTLIL::SigSpec repeat(முழு இலக்கம்) மாறிலி;

	வெறுமை reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(மாறிலி RTLIL::SigSpec &other) மாறிலி;
	bool operator ==(மாறிலி RTLIL::SigSpec &other) மாறிலி;
	உள்வரி bool operator !=(மாறிலி RTLIL::SigSpec &other) மாறிலி { திரும்பு !(*this == other); }

	bool is_wire() மாறிலி;
	bool is_chunk() மாறிலி;
	உள்வரி bool is_bit() மாறிலி { திரும்பு size() == 1; }

	bool known_driver() மாறிலி;

	bool is_fully_const() மாறிலி;
	bool is_fully_zero() மாறிலி;
	bool is_fully_ones() மாறிலி;
	bool is_fully_def() மாறிலி;
	bool is_fully_undef() மாறிலி;
	bool has_const() மாறிலி;
	bool has_const(State state) மாறிலி;
	bool has_marked_bits() மாறிலி;
	bool is_onehot(முழு *pos = nullptr) மாறிலி;

	bool as_bool() மாறிலி;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	முழு as_int(bool is_signed = false) மாறிலி;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) மாறிலி;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<முழு> try_as_int(bool is_signed = false) மாறிலி;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	முழு as_int_saturating(bool is_signed = false) மாறிலி;

	std::string as_string() மாறிலி;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() மாறிலி;
	RTLIL::Const as_const() மாறிலி;
	RTLIL::Wire *as_wire() மாறிலி;
	RTLIL::SigChunk as_chunk() மாறிலி;
	RTLIL::SigBit as_bit() மாறிலி;

	bool match(மாறிலி எழுத்து* pattern) மாறிலி;

	std::set<RTLIL::SigBit> to_sigbit_set() மாறிலி;
	pool<RTLIL::SigBit> to_sigbit_pool() மாறிலி;
	std::vector<RTLIL::SigBit> to_sigbit_vector() மாறிலி;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(மாறிலி RTLIL::SigSpec &other) மாறிலி;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(மாறிலி RTLIL::SigSpec &other) மாறிலி;

	நிலையான bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	நிலையான bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	நிலையான bool parse_rhs(மாறிலி RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() மாறிலி;
	operator std::vector<RTLIL::SigBit>() மாறிலி { திரும்பு to_sigbit_vector(); }
	மாறிலி RTLIL::SigBit &at(முழு offset, மாறிலி RTLIL::SigBit &defval) { திரும்பு offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(முழு offset) { திரும்பு (*this)[offset]; }
	RTLIL::SigBit at(முழு offset) மாறிலி { திரும்பு (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
		Hasher::hash_t val;
		எனில் (std::optional<Hasher::hash_t> current = hash_.பட())
			val = *current;
		இல்லையெனில்
			val = updhash();
		h.eat(val);
		திரும்பு h;
	}

#ifndef NDEBUG
	வெறுமை check(மாறிலி Module *mod = nullptr) மாறிலி;
#இல்லையெனில்
	வெறுமை check(மாறிலி Module *mod = nullptr) மாறிலி { (வெறுமை)mod; }
#endif
};

கட்டமைப்பு RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) மாறிலி;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) மாறிலி;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) மாறிலி;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) மாறிலி;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	வெறுமை optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() மாறிலி {
		திரும்பு full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> வெறுமை select(T1 *module) {
		எனில் (!selects_all() && selected_modules.மொத்தம்(module->name) == 0) {
			IdString name = module->name;
			selected_modules.செருகு(name);
			selected_members.erase(name);
			எனில் (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> வெறுமை select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> வெறுமை select(T1 *module, RTLIL::IdString member) {
		எனில் (!selects_all() && selected_modules.மொத்தம்(module->name.ref()) == 0) {
			selected_members[module->name.ref()].செருகு(member);
			எனில் (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() மாறிலி {
		திரும்பு !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	வெறுமை clear();

	// create a new selection which is empty
	நிலையான Selection EmptySelection(RTLIL::Design *design = nullptr) { திரும்பு Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	நிலையான Selection FullSelection(RTLIL::Design *design = nullptr) { திரும்பு Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	நிலையான Selection CompleteSelection(RTLIL::Design *design = nullptr) { திரும்பு Selection(true, true, design); };
};

கட்டமைப்பு RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

	Monitor() {
		நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
		hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
		hashidx_ = hashidx_மொத்தம்;
	}

	மெய்நிகர் ~Monitor() { }
	மெய்நிகர் வெறுமை notify_module_add(RTLIL::Module*) { }
	மெய்நிகர் வெறுமை notify_module_del(RTLIL::Module*) { }
	மெய்நிகர் வெறுமை notify_connect(RTLIL::Cell*, RTLIL::IdString, மாறிலி RTLIL::SigSpec&, மாறிலி RTLIL::SigSpec&) { }
	மெய்நிகர் வெறுமை notify_connect(RTLIL::Module*, மாறிலி RTLIL::SigSig&) { }
	மெய்நிகர் வெறுமை notify_connect(RTLIL::Module*, மாறிலி std::vector<RTLIL::SigSig>&) { }
	மெய்நிகர் வெறுமை notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
கட்டமைப்பு define_map_t;

template<typename N>
உள்வரி constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, மாறிலி எழுத்து*> || std::is_same_v<std::decay_t<N>, எழுத்து*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, முழு> = 0

template<typename N>
உள்வரி constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, மாறிலி எழுத்து *> || std::is_same_v<std::decay_t<N>, எழுத்து *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, முழு> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ திரும்பு _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ திரும்பு _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

கட்டமைப்பு RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	வெறுமை bufNormalize(bool enable=true);

	முழு refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

தனிப்பட்ட:
	TwinePool twines_;

பொது:
	TwinePool &twines() { திரும்பு twines_; }
	மாறிலி TwinePool &twines() மாறிலி { திரும்பு twines_; }

	வெறுமை absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_முனை>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_அடுக்கு;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	மாறிலி RTLIL::Module *module(RTLIL::IdString name) மாறிலி;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ திரும்பு module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> மாறிலி RTLIL::Module *module(N name) மாறிலி
		{ திரும்பு module(twines().find(std::string_view(name))); }
	RTLIL::Module *உச்சி_module() மாறிலி;

	bool has(RTLIL::IdString id) மாறிலி {
		twines_.check_owned(id);
		திரும்பு modules_.மொத்தம்(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) மாறிலி
		{ திரும்பு has(twines().find(std::string_view(name))); }

	வெறுமை add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	வெறுமை remove(RTLIL::Module *module);
	வெறுமை rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	வெறுமை scratchpad_unset(மாறிலி std::string &varname);

	வெறுமை scratchpad_set_int(மாறிலி std::string &varname, முழு மதிப்பு);
	வெறுமை scratchpad_set_bool(மாறிலி std::string &varname, bool மதிப்பு);
	வெறுமை scratchpad_set_string(மாறிலி std::string &varname, std::string மதிப்பு);

	முழு scratchpad_get_int(மாறிலி std::string &varname, முழு default_மதிப்பு_2 = 0) மாறிலி;
	bool scratchpad_get_bool(மாறிலி std::string &varname, bool default_மதிப்பு_2 = false) மாறிலி;
	std::string scratchpad_get_string(மாறிலி std::string &varname, மாறிலி std::string &default_மதிப்பு_2 = std::string()) மாறிலி;

	வெறுமை sort();
	வெறுமை sort_modules();
	வெறுமை check();
	வெறுமை optimize();

	வெறுமை clone_into(RTLIL::Design *dst) மாறிலி;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) மாறிலி;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) மாறிலி;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) மாறிலி;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) மாறிலி;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) மாறிலி;

	// push the given selection to the selection stack
	வெறுமை தள்ளு_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	வெறுமை தள்ளு_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	வெறுமை தள்ளு_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	வெறுமை தள்ளு_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	வெறுமை எடு_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		திரும்பு selection_அடுக்கு.back();
	}

	// get the current selection
	மாறிலி RTLIL::Selection &selection() மாறிலி {
		திரும்பு selection_அடுக்கு.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() மாறிலி {
		திரும்பு selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) மாறிலி {
		திரும்பு selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) மாறிலி {
		திரும்பு selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> வெறுமை select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> வெறுமை select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) மாறிலி;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() மாறிலி { திரும்பு selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() மாறிலி { திரும்பு selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() மாறிலி { திரும்பு selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() மாறிலி { திரும்பு selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() மாறிலி { திரும்பு selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) மாறிலி { திரும்பு selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() மாறிலி { திரும்பு selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() மாறிலி { திரும்பு selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	நிலையான std::map<unsigned முழு, RTLIL::Design*> *get_all_designs(வெறுமை);

	std::string to_rtlil_str(bool only_selected = true) மாறிலி;
};

கட்டமைப்பு RTLIL::Module : பொது RTLIL::AttrObject
{
	friend கட்டமைப்பு RTLIL::Cell;
	friend கட்டமைப்பு RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

பாதுகாக்கப்பட்ட:
	வெறுமை add(RTLIL::Wire *wire);
	வெறுமை add(RTLIL::Cell *cell);
	வெறுமை add(RTLIL::Process *process);

பொது:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() மாறிலி { திரும்பு design->twines(); }

	முழு refcount_wires_;
	முழு refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_மதிப்புகள்;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	மெய்நிகர் ~Module();
	மெய்நிகர் RTLIL::IdString derive(RTLIL::Design *design, மாறிலி dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	மெய்நிகர் RTLIL::IdString derive(RTLIL::Design *design, மாறிலி dict<RTLIL::IdString, RTLIL::Const> &parameters, மாறிலி dict<RTLIL::IdString, RTLIL::Module*> &interfaces, மாறிலி dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	மெய்நிகர் size_t மொத்தம்_id(RTLIL::IdString id);
	மெய்நிகர் வெறுமை expand_interfaces(RTLIL::Design *design, மாறிலி dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	மெய்நிகர் bool reprocess_if_necessary(RTLIL::Design *design);

	மெய்நிகர் வெறுமை sort();
	மெய்நிகர் வெறுமை check();
	மெய்நிகர் வெறுமை optimize();
	மெய்நிகர் வெறுமை makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) மாறிலி {
		திரும்பு get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	வெறுமை connect(மாறிலி RTLIL::SigSig &conn);
	வெறுமை connect(மாறிலி RTLIL::SigSpec &lhs, மாறிலி RTLIL::SigSpec &rhs);
	வெறுமை new_connections(மாறிலி std::vector<RTLIL::SigSig> &new_conn);
	மாறிலி std::vector<RTLIL::SigSig> &connections() மாறிலி;

	std::vector<RTLIL::IdString> ports;
	வெறுமை fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	வெறுமை bufNormalize();

	template<typename T> வெறுமை rewrite_sigspecs(T &functor);
	template<typename T> வெறுமை rewrite_sigspecs2(T &functor);
	வெறுமை cloneInto(RTLIL::Module *new_mod) மாறிலி;
	மெய்நிகர் RTLIL::Module *clone() மாறிலி;
	மெய்நிகர் RTLIL::Module *clone(RTLIL::Design *dst) மாறிலி;
	மெய்நிகர் RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) மாறிலி;

	bool has_memories() மாறிலி;
	bool has_processes() மாறிலி;

	bool has_memories_warn() மாறிலி;
	bool has_processes_warn() மாறிலி;

	bool is_selected() மாறிலி;
	bool is_selected_whole() மாறிலி;

	std::vector<RTLIL::Wire*> selected_wires() மாறிலி;
	std::vector<RTLIL::Cell*> selected_cells() மாறிலி;
	std::vector<RTLIL::Memory*> selected_memories() மாறிலி;
	std::vector<RTLIL::Process*> selected_processes() மாறிலி;
	std::vector<RTLIL::SelectedMember> selected_members() மாறிலி;

	template<typename T> bool selected(T *member) மாறிலி {
		திரும்பு design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		எனில் (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		திரும்பு it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		எனில் (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		திரும்பு it == cells_.end() ? nullptr : it->second;
	}

	மாறிலி RTLIL::Wire* wire(RTLIL::IdString id) மாறிலி{
		எனில் (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		திரும்பு it == wires_.end() ? nullptr : it->second;
	}
	மாறிலி RTLIL::Cell* cell(RTLIL::IdString id) மாறிலி {
		எனில் (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		திரும்பு it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ திரும்பு wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ திரும்பு cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> மாறிலி RTLIL::Wire *wire(N name) மாறிலி
		{ திரும்பு wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> மாறிலி RTLIL::Cell *cell(N name) மாறிலி
		{ திரும்பு cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { திரும்பு RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	முழு wires_size() மாறிலி { திரும்பு wires_.size(); }
	RTLIL::Wire* wire_at(முழு index) மாறிலி { திரும்பு wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { திரும்பு RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	முழு cells_size() மாறிலி { திரும்பு cells_.size(); }
	RTLIL::Cell* cell_at(முழு index) மாறிலி { திரும்பு cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	வெறுமை remove(மாறிலி pool<RTLIL::Wire*> &wires);
	வெறுமை remove(RTLIL::Cell *cell);
	வெறுமை remove(RTLIL::Memory *memory);
	வெறுமை remove(RTLIL::Process *process);

	வெறுமை rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	வெறுமை rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	வெறுமை rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	வெறுமை swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	வெறுமை swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, முழு &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, முழு width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, மாறிலி RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, மாறிலி RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { திரும்பு addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, மாறிலி RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, மாறிலி RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி RTLIL::SigSpec &sig_x, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, RTLIL::Const offset, மாறிலி std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, RTLIL::Const lut, மாறிலி std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr, மாறிலி RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_d,   மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en,  மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, RTLIL::Const arst_மதிப்பு, bool clk_polarity = true, bool arst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst,  மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, RTLIL::Const arst_மதிப்பு, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_aload,  மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, RTLIL::Const srst_மதிப்பு, bool clk_polarity = true, bool srst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst,  மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, RTLIL::Const srst_மதிப்பு, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, RTLIL::Const srst_மதிப்பு, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool en_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, RTLIL::Const arst_மதிப்பு, bool en_polarity = true, bool arst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_s, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_s, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி RTLIL::SigBit &sig_d, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி RTLIL::SigBit &sig_d, மாறிலி RTLIL::SigBit &sig_y, மாறிலி std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
			மாறிலி RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			bool arst_மதிப்பு = false, bool clk_polarity = true, bool arst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			bool arst_மதிப்பு = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			மாறிலி RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			மாறிலி RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			bool srst_மதிப்பு = false, bool clk_polarity = true, bool srst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			bool srst_மதிப்பு = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			bool srst_மதிப்பு = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool en_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
			bool arst_மதிப்பு = false, bool en_polarity = true, bool arst_polarity = true, மாறிலி std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, மாறிலி std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, bool is_signed = false, மாறிலி std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_s, மாறிலி std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_s, மாறிலி std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி RTLIL::SigBit &sig_d, மாறிலி std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig_a, மாறிலி RTLIL::SigBit &sig_b, மாறிலி RTLIL::SigBit &sig_c, மாறிலி RTLIL::SigBit &sig_d, மாறிலி std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, முழு width = 1, மாறிலி std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, முழு width = 1, மாறிலி std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, முழு width = 1, மாறிலி std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, முழு width = 1, மாறிலி std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, மாறிலி std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_e, மாறிலி std::string &src = "");
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

	std::string to_rtlil_str() மாறிலி;
#ifdef YOSYS_ENABLE_PYTHON
	நிலையான std::map<unsigned முழு, RTLIL::Module*> *get_all_modules(வெறுமை);
#endif
};

கட்டமைப்பு RTLIL::Wire : பொது RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

பாதுகாக்கப்பட்ட:
	// use module->addWire() and module->remove() to create or destroy wires
	friend கட்டமைப்பு RTLIL::Module;
	Wire();
	~Wire();

	friend கட்டமைப்பு RTLIL::Design;
	friend கட்டமைப்பு RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

பொது:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	வெறுமை operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	முழு width, start_offset, port_id;
	bool port_உள்ளீடு, port_output, upto, is_signed;

	RTLIL::Design *design() மாறிலி { திரும்பு module ? module->design : nullptr; }
	TwinePool &twines() மாறிலி { திரும்பு module->twines(); }

	bool known_driver() மாறிலி { திரும்பு driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() மாறிலி    { log_assert(driverCell_); திரும்பு driverCell_; };
	RTLIL::IdString driverPort() மாறிலி { log_assert(driverCell_); திரும்பு driverPort_; };

	முழு from_hdl_index(முழு hdl_index) {
		முழு zero_index = hdl_index - start_offset;
		முழு rtlil_index = upto ? width - 1 - zero_index : zero_index;
		திரும்பு rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	முழு to_hdl_index(முழு rtlil_index) {
		எனில் (rtlil_index < 0 || rtlil_index >= width)
			திரும்பு INT_MIN;
		முழு zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		திரும்பு zero_index + start_offset;
	}

	std::string to_rtlil_str() மாறிலி;

#ifdef YOSYS_ENABLE_PYTHON
	நிலையான std::map<unsigned முழு, RTLIL::Wire*> *get_all_wires(வெறுமை);
#endif
};

உள்வரி முழு GetSize(RTLIL::Wire *wire) {
	திரும்பு wire->width;
}

கட்டமைப்பு RTLIL::Memory : பொது RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() மாறிலி { திரும்பு module ? module->design : nullptr; }
	TwinePool &twines() மாறிலி { திரும்பு module->twines(); }

	RTLIL::MemoryNameId name;

	முழு width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	நிலையான std::map<unsigned முழு, RTLIL::Memory*> *get_all_memorys(வெறுமை);
#endif

	std::string to_rtlil_str() மாறிலி;
};

கட்டமைப்பு RTLIL::Cell : பொது RTLIL::AttrObject
{
தனிப்பட்ட:
	வெறுமை initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &சமிக்ஞை, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
பொது:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

பாதுகாக்கப்பட்ட:
	// use module->addCell() and module->remove() to create or destroy cells
	friend கட்டமைப்பு RTLIL::Module;
	Cell();
	~Cell();

பொது:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	வெறுமை operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() மாறிலி { திரும்பு module ? module->design : nullptr; }
	TwinePool &twines() மாறிலி { திரும்பு module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) மாறிலி;
	வெறுமை unsetPort(RTLIL::IdString portname);
	வெறுமை setPort(RTLIL::IdString portname, RTLIL::SigSpec சமிக்ஞை);
	template<typename N, YS_NAME_STRING(N)> வெறுமை setPort(N portname, RTLIL::SigSpec சமிக்ஞை)
		{ setPort(module->twines().add(std::move(portname)), std::move(சமிக்ஞை)); }
	மாறிலி RTLIL::SigSpec &getPort(RTLIL::IdString portname) மாறிலி;
	மாறிலி dict<RTLIL::IdString, RTLIL::SigSpec> &connections() மாறிலி;

	bool known() மாறிலி;
	bool உள்ளீடு(RTLIL::IdString portname) மாறிலி;
	bool output(RTLIL::IdString portname) மாறிலி;
	PortDir port_dir(RTLIL::IdString portname) மாறிலி;

	bool hasParam(RTLIL::IdString paramname) மாறிலி;
	வெறுமை unsetParam(RTLIL::IdString paramname);
	வெறுமை setParam(RTLIL::IdString paramname, RTLIL::Const மதிப்பு);
	மாறிலி RTLIL::Const &getParam(RTLIL::IdString paramname) மாறிலி;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) மாறிலி
		{ திரும்பு hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> வெறுமை unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> வெறுமை setParam(N name, RTLIL::Const மதிப்பு)
		{ setParam(module->twines().add(std::move(name)), std::move(மதிப்பு)); }
	template<typename N, YS_NAME_STRING(N)> மாறிலி RTLIL::Const &getParam(N name) மாறிலி
		{ திரும்பு getParam(module->twines().find(std::string_view(name))); }

	வெறுமை sort();
	வெறுமை check();
	வெறுமை fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() மாறிலி;

	template<typename T> வெறுமை rewrite_sigspecs(T &functor);
	template<typename T> வெறுமை rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	நிலையான std::map<unsigned முழு, RTLIL::Cell*> *get_all_cells(வெறுமை);
#endif

	bool has_memid() மாறிலி;
	bool is_mem_cell() மாறிலி;
	bool is_builtin_ff() மாறிலி;

	std::string to_rtlil_str() மாறிலி;
};

கட்டமைப்பு RTLIL::CaseRule : பொது RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() மாறிலி;

	template<typename T> வெறுமை rewrite_sigspecs(T &functor);
	template<typename T> வெறுமை rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() மாறிலி;
};

கட்டமைப்பு RTLIL::SwitchRule : பொது RTLIL::AttrObject
{
	RTLIL::SigSpec சமிக்ஞை;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() மாறிலி;

	template<typename T> வெறுமை rewrite_sigspecs(T &functor);
	template<typename T> வெறுமை rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() மாறிலி;
};

கட்டமைப்பு RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

கட்டமைப்பு RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec சமிக்ஞை;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> வெறுமை rewrite_sigspecs(T &functor);
	template<typename T> வெறுமை rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() மாறிலி;
};

கட்டமைப்பு RTLIL::Process : பொது RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(hashidx_); திரும்பு h; }

பாதுகாக்கப்பட்ட:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend கட்டமைப்பு RTLIL::Module;
	Process();
	~Process();

பொது:
	RTLIL::Module *module;
	RTLIL::CaseRule வேர்_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() மாறிலி { திரும்பு module ? module->design : nullptr; }
	TwinePool &twines() மாறிலி { திரும்பு module->twines(); }

	template<typename T> வெறுமை rewrite_sigspecs(T &functor);
	template<typename T> வெறுமை rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() மாறிலி;

	std::string to_rtlil_str() மாறிலி;
};

உள்வரி RTLIL::SigBit::SigBit() : wire(வெற்று), data(RTLIL::State::S0) { }
உள்வரி RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(வெற்று), data(bit) { }
உள்வரி RTLIL::SigBit::SigBit(bool bit) : wire(வெற்று), data(bit ? State::S1 : State::S0) { }
உள்வரி RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
உள்வரி RTLIL::SigBit::SigBit(RTLIL::Wire *wire, முழு offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
உள்வரி RTLIL::SigBit::SigBit(மாறிலி RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); எனில் (wire) offset = chunk.offset; இல்லையெனில் data = chunk.data[0]; }
உள்வரி RTLIL::SigBit::SigBit(மாறிலி RTLIL::SigChunk &chunk, முழு index) : wire(chunk.wire) { எனில் (wire) offset = chunk.offset + index; இல்லையெனில் data = chunk.data[index]; }

உள்வரி bool RTLIL::SigBit::operator<(மாறிலி RTLIL::SigBit &other) மாறிலி {
	எனில் (wire == other.wire)
		திரும்பு wire ? (offset < other.offset) : (data < other.data);
	எனில் (wire != nullptr && other.wire != nullptr)
		திரும்பு wire->name < other.wire->name;
	திரும்பு (wire != nullptr) < (other.wire != nullptr);
}

உள்வரி bool RTLIL::SigBit::operator==(மாறிலி RTLIL::SigBit &other) மாறிலி {
	திரும்பு (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

உள்வரி bool RTLIL::SigBit::operator!=(மாறிலி RTLIL::SigBit &other) மாறிலி {
	திரும்பு (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

உள்வரி Hasher RTLIL::SigBit::hash_into(Hasher h) மாறிலி {
	எனில் (wire) {
		h.eat(offset);
		h.eat(wire->name);
		திரும்பு h;
	}
	h.eat(data);
	திரும்பு h;
}


உள்வரி Hasher RTLIL::SigBit::hash_உச்சி() மாறிலி {
	Hasher h;
	எனில் (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		திரும்பு h;
	}
	h.force(data);
	திரும்பு h;
}

உள்வரி RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() மாறிலி {
	திரும்பு (*sig_p)[index];
}

உள்வரி மாறிலி RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	திரும்பு bit;
}

உள்வரி RTLIL::SigBit::SigBit(மாறிலி RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
வெறுமை RTLIL::Module::rewrite_sigspecs(T &functor)
{
	க்கு (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	க்கு (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	க்கு (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
வெறுமை RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	க்கு (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	க்கு (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	க்கு (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
வெறுமை RTLIL::Cell::rewrite_sigspecs(T &functor) {
	க்கு (auto &it : connections_)
		functor(it.second);
}

template<typename T>
வெறுமை RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	க்கு (auto &it : connections_)
		functor(it.second);
}

template<typename T>
வெறுமை RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	க்கு (auto &it : compare)
		functor(it);
	க்கு (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	க்கு (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
வெறுமை RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	க்கு (auto &it : compare)
		functor(it);
	க்கு (auto &it : actions) {
		functor(it.first, it.second);
	}
	க்கு (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
வெறுமை RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(சமிக்ஞை);
	க்கு (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
வெறுமை RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(சமிக்ஞை);
	க்கு (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
வெறுமை RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(சமிக்ஞை);
	க்கு (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	க்கு (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
வெறுமை RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(சமிக்ஞை);
	க்கு (auto &it : actions) {
		functor(it.first, it.second);
	}
	க்கு (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
வெறுமை RTLIL::Process::rewrite_sigspecs(T &functor)
{
	வேர்_case.rewrite_sigspecs(functor);
	க்கு (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
வெறுமை RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	வேர்_case.rewrite_sigspecs2(functor);
	க்கு (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#அந்தர்பாவ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
