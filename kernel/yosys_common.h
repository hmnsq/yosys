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

#ifndef YOSYS_COMMON_H
#define YOSYS_COMMON_H

#அந்தர்பாவ <array>
#அந்தர்பாவ <atomic>
#அந்தர்பாவ <map>
#அந்தர்பாவ <set>
#அந்தர்பாவ <tuple>
#அந்தர்பாவ <vector>
#அந்தர்பாவ <string>
#அந்தர்பாவ <algorithm>
#அந்தர்பாவ <functional>
#அந்தர்பாவ <unordered_map>
#அந்தர்பாவ <unordered_set>
#அந்தர்பாவ <initializer_list>
#அந்தர்பாவ <variant>
#அந்தர்பாவ <optional>
#அந்தர்பாவ <stdexcept>
#அந்தர்பாவ <memory>
#அந்தர்பாவ <cmath>
#அந்தர்பாவ <cstddef>

#அந்தர்பாவ <sstream>
#அந்தர்பாவ <fstream>
#அந்தர்பாவ <istream>
#அந்தர்பாவ <ostream>
#அந்தர்பாவ <iostream>

#அந்தர்பாவ <version>
#எனில் __cpp_lib_source_location >= 201907L
	#அந்தர்பாவ <source_location>
	using std::source_location;
#இல்லையெனில்
	#அந்தர்பாவ <experimental/source_location>
	using std::experimental::source_location;
#endif

#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set க்கு your build configuration.
#endif

#அந்தர்பாவ "kernel/yosys_config.h"

#define FRIEND_TEST(test_case_name, test_name) \
  friend class test_case_name##_##test_name##_Test

#ifdef _WIN32
#  undef NOMINMAX
#  define NOMINMAX 1
#  undef YY_NO_UNISTD_H
#  define YY_NO_UNISTD_H 1

#  include <windows.h>
#  include <io.h>
#  include <direct.h>

#  define strtok_r strtok_s
#  define strdup _strdup
#  define getcwd _getcwd
#  define mkdir _mkdir
#  define popen _popen
#  define pclose _pclose

#  ifndef __MINGW32__
#    define PATH_MAX MAX_PATH
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#endif

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif


#define YOSYS_NAMESPACE          Yosys
#define PRIVATE_NAMESPACE_BEGIN  namespace {
#define PRIVATE_NAMESPACE_END    }
#define YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#define YOSYS_NAMESPACE_END      }
#define YOSYS_NAMESPACE_PREFIX   Yosys::
#define USING_YOSYS_NAMESPACE    using namespace Yosys;

#எனில் defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#அல்லதெனில் defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#இல்லையெனில்
#  define YS_ATTRIBUTE(...)
#endif

#எனில் defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#இல்லையெனில்
#  define YS_MAYBE_UNUSED
#endif

#எனில் defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#இல்லையெனில்
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#எனில் __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#இல்லையெனில்
#  error "C++20 or later compatible compiler is required"
#endif

#எனில் defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#இல்லையெனில்
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#இல்லையெனில்
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] வெறுமை log_yosys_abort_message(std::string_view file, முழு வரி, std::string_view func, std::string_view message);

[[noreturn]]
வெறுமை log_assert_failure(மாறிலி எழுத்து *கோவை, source_location location);
#ifndef NDEBUG
நிலையான உள்வரி வெறுமை log_assert_worker(bool cond, மாறிலி எழுத்து *கோவை, source_location location = source_location::current()) {
	எனில் (!cond) log_assert_failure(கோவை, location);
}
#  define log_assert(assert_கோவை) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_கோவை, #_assert_expr_)
#இல்லையெனில்
#  define log_assert(assert_கோவை) செய் { எனில் (0) { (வெறுமை)(assert_கோவை); } } போது(0)
#endif
YOSYS_NAMESPACE_END

#அந்தர்பாவ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#இல்லையெனில்
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#endif


using std::vector;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::run_hash;
using hashlib::hash_ops;
using hashlib::mkhash_xorshift;
using hashlib::dict;
using hashlib::idict;
using hashlib::pool;
using hashlib::mfp;

// A primitive shared string implementation that does not
// move its .c_str() when the object is copied or moved.
கட்டமைப்பு shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(மாறிலி எழுத்து *s) { content = std::shared_ptr<string>(new string(s)); }
	மாறிலி எழுத்து *c_str() மாறிலி { திரும்பு content->c_str(); }
	மாறிலி string &str() மாறிலி { திரும்பு *content; }
	bool operator==(மாறிலி shared_str &other) மாறிலி { திரும்பு *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
		h.eat(*content);
		திரும்பு h;
	}
};

namespace RTLIL {
	கட்டமைப்பு Const;
	கட்டமைப்பு SigBit;
	கட்டமைப்பு SigSpec;
	கட்டமைப்பு Wire;
	கட்டமைப்பு Cell;
	கட்டமைப்பு Memory;
	கட்டமைப்பு Process;
	கட்டமைப்பு Module;
	கட்டமைப்பு Design;
	கட்டமைப்பு Monitor;
	கட்டமைப்பு Selection;
	கட்டமைப்பு SigChunk;
	கட்டமைப்பு PooledName;
	எண்ணிக்கை State : unsigned எழுத்து;

	வகைவரை std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	கட்டமைப்பு Ast_முனை;
}

using RTLIL::Const;
using RTLIL::SigBit;
using RTLIL::SigSpec;
using RTLIL::Wire;
using RTLIL::Cell;
using RTLIL::Module;
using RTLIL::Design;

using RTLIL::State;
using RTLIL::SigChunk;
using RTLIL::SigSig;
using RTLIL::PooledName;

namespace hashlib {
	template<> கட்டமைப்பு hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<AST::Ast_முனை*> : hash_obj_ops {};

	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Wire*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Cell*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Memory*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Process*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Module*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Design*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி RTLIL::Monitor*> : hash_obj_ops {};
	template<> கட்டமைப்பு hash_ops<மாறிலி AST::Ast_முனை*> : hash_obj_ops {};
}

வெறுமை memhasher_on();
வெறுமை memhasher_off();
வெறுமை memhasher_do();

வெளி bool memhasher_active;
உள்வரி வெறுமை memhasher() { எனில் (memhasher_active) memhasher_do(); }

வெறுமை yosys_banner();
முழு ceil_log2(முழு x) YS_ATTRIBUTE(மாறிலி);

template<typename T> முழு GetSize(மாறிலி T &obj) { திரும்பு obj.size(); }
உள்வரி முழு GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
கட்டமைப்பு Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	நிலையான bool active() { திரும்பு active_; }
தனிப்பட்ட:
	நிலையான bool active_;
};

கட்டமைப்பு Autoidx {
	Autoidx(முழு மதிப்பு) : மதிப்பு(மதிப்பு) {}
	operator முழு() மாறிலி { திரும்பு மதிப்பு; }
	வெறுமை ensure_at_least(முழு v);
	முழு operator++(முழு);
தனிப்பட்ட:
	முழு மதிப்பு;
};

வெளி Autoidx autoidx;
வெளி முழு yosys_xtrace;
வெளி bool yosys_write_versions;

மாறிலி std::string *create_id_prefix(std::string_view file, முழு வரி, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> மாறிலி std::string * { \
		நிலையான std::unique_ptr<மாறிலி std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.வரி(), func)); \
		திரும்பு prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> மாறிலி std::string * { \
		நிலையான std::unique_ptr<மாறிலி std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.வரி(), func)); \
		திரும்பு prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
