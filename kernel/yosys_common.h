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

#ಅನ್ತರ್ಭಾವ <array>
#ಅನ್ತರ್ಭಾವ <atomic>
#ಅನ್ತರ್ಭಾವ <map>
#ಅನ್ತರ್ಭಾವ <set>
#ಅನ್ತರ್ಭಾವ <tuple>
#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <functional>
#ಅನ್ತರ್ಭಾವ <unordered_map>
#ಅನ್ತರ್ಭಾವ <unordered_set>
#ಅನ್ತರ್ಭಾವ <initializer_list>
#ಅನ್ತರ್ಭಾವ <variant>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <stdexcept>
#ಅನ್ತರ್ಭಾವ <memory>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ <cstddef>

#ಅನ್ತರ್ಭಾವ <sstream>
#ಅನ್ತರ್ಭಾವ <fstream>
#ಅನ್ತರ್ಭಾವ <istream>
#ಅನ್ತರ್ಭಾವ <ostream>
#ಅನ್ತರ್ಭಾವ <iostream>

#ಅನ್ತರ್ಭಾವ <version>
#ಯದಿ __cpp_lib_source_location >= 201907L
	#ಅನ್ತರ್ಭಾವ <source_location>
	using std::source_location;
#ಅನ್ಯಥಾ
	#ಅನ್ತರ್ಭಾವ <experimental/source_location>
	using std::experimental::source_location;
#endif

#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set ಪ್ರತಿ your build configuration.
#endif

#ಅನ್ತರ್ಭಾವ "kernel/yosys_config.h"

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

#ಯದಿ defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#ನೋಚೇದ್ಯದಿ defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#ಅನ್ಯಥಾ
#  define YS_ATTRIBUTE(...)
#endif

#ಯದಿ defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#ಅನ್ಯಥಾ
#  define YS_MAYBE_UNUSED
#endif

#ಯದಿ defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#ಅನ್ಯಥಾ
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#ಯದಿ __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#ಅನ್ಯಥಾ
#  error "C++20 or later compatible compiler is required"
#endif

#ಯದಿ defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#ಅನ್ಯಥಾ
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#ಅನ್ಯಥಾ
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] ಶೂನ್ಯ log_yosys_abort_message(std::string_view file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, std::string_view func, std::string_view message);

[[noreturn]]
ಶೂನ್ಯ log_assert_failure(ಧ್ರುವ ವರ್ಣ *ಅಭಿವ್ಯಕ್ತಿ, source_location location);
#ifndef NDEBUG
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ log_assert_worker(bool cond, ಧ್ರುವ ವರ್ಣ *ಅಭಿವ್ಯಕ್ತಿ, source_location location = source_location::current()) {
	ಯದಿ (!cond) log_assert_failure(ಅಭಿವ್ಯಕ್ತಿ, location);
}
#  define log_assert(assert_ಅಭಿವ್ಯಕ್ತಿ) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_ಅಭಿವ್ಯಕ್ತಿ, #_assert_expr_)
#ಅನ್ಯಥಾ
#  define log_assert(assert_ಅಭಿವ್ಯಕ್ತಿ) ಕುರು { ಯದಿ (0) { (ಶೂನ್ಯ)(assert_ಅಭಿವ್ಯಕ್ತಿ); } } ಯಾವತ್(0)
#endif
YOSYS_NAMESPACE_END

#ಅನ್ತರ್ಭಾವ "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#ಅನ್ಯಥಾ
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
ಸಂರಚನಾ shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ಧ್ರುವ ವರ್ಣ *s) { content = std::shared_ptr<string>(new string(s)); }
	ಧ್ರುವ ವರ್ಣ *c_str() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ content->c_str(); }
	ಧ್ರುವ string &str() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *content; }
	bool operator==(ಧ್ರುವ shared_str &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
		h.eat(*content);
		ಪ್ರತ್ಯಾಗಮ h;
	}
};

namespace RTLIL {
	ಸಂರಚನಾ Const;
	ಸಂರಚನಾ SigBit;
	ಸಂರಚನಾ SigSpec;
	ಸಂರಚನಾ Wire;
	ಸಂರಚನಾ Cell;
	ಸಂರಚನಾ Memory;
	ಸಂರಚನಾ Process;
	ಸಂರಚನಾ Module;
	ಸಂರಚನಾ Design;
	ಸಂರಚನಾ Monitor;
	ಸಂರಚನಾ Selection;
	ಸಂರಚನಾ SigChunk;
	ಸಂರಚನಾ PooledName;
	ಗಣನಾ State : unsigned ವರ್ಣ;

	ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ಸಂರಚನಾ Ast_ಗಂಟು;
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
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<AST::Ast_ಗಂಟು*> : hash_obj_ops {};

	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Wire*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Cell*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Memory*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Process*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Module*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Design*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ RTLIL::Monitor*> : hash_obj_ops {};
	template<> ಸಂರಚನಾ hash_ops<ಧ್ರುವ AST::Ast_ಗಂಟು*> : hash_obj_ops {};
}

ಶೂನ್ಯ memhasher_on();
ಶೂನ್ಯ memhasher_off();
ಶೂನ್ಯ memhasher_do();

ಬಾಹ್ಯ bool memhasher_active;
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ memhasher() { ಯದಿ (memhasher_active) memhasher_do(); }

ಶೂನ್ಯ yosys_banner();
ಪೂರ್ಣಾಙ್ಕ ceil_log2(ಪೂರ್ಣಾಙ್ಕ x) YS_ATTRIBUTE(ಧ್ರುವ);

template<typename T> ಪೂರ್ಣಾಙ್ಕ GetSize(ಧ್ರುವ T &obj) { ಪ್ರತ್ಯಾಗಮ obj.size(); }
ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ಸಂರಚನಾ Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ಸ್ಥಿರ bool active() { ಪ್ರತ್ಯಾಗಮ active_; }
ಗುಪ್ತ:
	ಸ್ಥಿರ bool active_;
};

ಸಂರಚನಾ Autoidx {
	Autoidx(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ) : ಮೌಲ್ಯ(ಮೌಲ್ಯ) {}
	operator ಪೂರ್ಣಾಙ್ಕ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ; }
	ಶೂನ್ಯ ensure_at_least(ಪೂರ್ಣಾಙ್ಕ v);
	ಪೂರ್ಣಾಙ್ಕ operator++(ಪೂರ್ಣಾಙ್ಕ);
ಗುಪ್ತ:
	ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;
};

ಬಾಹ್ಯ Autoidx autoidx;
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ yosys_xtrace;
ಬಾಹ್ಯ bool yosys_write_versions;

ಧ್ರುವ std::string *create_id_prefix(std::string_view file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ಧ್ರುವ std::string * { \
		ಸ್ಥಿರ std::unique_ptr<ಧ್ರುವ std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.ಸಾಲು(), func)); \
		ಪ್ರತ್ಯಾಗಮ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ಧ್ರುವ std::string * { \
		ಸ್ಥಿರ std::unique_ptr<ಧ್ರುವ std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.ಸಾಲು(), func)); \
		ಪ್ರತ್ಯಾಗಮ prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
