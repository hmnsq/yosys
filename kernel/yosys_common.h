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

#অন্তর্ভাব <array>
#অন্তর্ভাব <atomic>
#অন্তর্ভাব <map>
#অন্তর্ভাব <set>
#অন্তর্ভাব <tuple>
#অন্তর্ভাব <vector>
#অন্তর্ভাব <string>
#অন্তর্ভাব <algorithm>
#অন্তর্ভাব <functional>
#অন্তর্ভাব <unordered_map>
#অন্তর্ভাব <unordered_set>
#অন্তর্ভাব <initializer_list>
#অন্তর্ভাব <variant>
#অন্তর্ভাব <optional>
#অন্তর্ভাব <stdexcept>
#অন্তর্ভাব <memory>
#অন্তর্ভাব <cmath>
#অন্তর্ভাব <cstddef>

#অন্তর্ভাব <sstream>
#অন্তর্ভাব <fstream>
#অন্তর্ভাব <istream>
#অন্তর্ভাব <ostream>
#অন্তর্ভাব <iostream>

#অন্তর্ভাব <version>
#যদি __cpp_lib_source_location >= 201907L
	#অন্তর্ভাব <source_location>
	using std::source_location;
#অন্যথা
	#অন্তর্ভাব <experimental/source_location>
	using std::experimental::source_location;
#endif

#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set প্রতি your build configuration.
#endif

#অন্তর্ভাব "kernel/yosys_config.h"

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

#যদি defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#নোচেদ্যদি defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#অন্যথা
#  define YS_ATTRIBUTE(...)
#endif

#যদি defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#অন্যথা
#  define YS_MAYBE_UNUSED
#endif

#যদি defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#অন্যথা
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#যদি __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#অন্যথা
#  error "C++20 or later compatible compiler is required"
#endif

#যদি defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#অন্যথা
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#অন্যথা
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] শূন্য log_yosys_abort_message(std::string_view file, পূর্ণাঙ্ক সারি, std::string_view func, std::string_view message);

[[noreturn]]
শূন্য log_assert_failure(ধ্রুব বর্ণ *রাশিমালা, source_location location);
#ifndef NDEBUG
স্থির অন্তঃস্থ শূন্য log_assert_worker(bool cond, ধ্রুব বর্ণ *রাশিমালা, source_location location = source_location::current()) {
	যদি (!cond) log_assert_failure(রাশিমালা, location);
}
#  define log_assert(assert_রাশিমালা) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_রাশিমালা, #_assert_expr_)
#অন্যথা
#  define log_assert(assert_রাশিমালা) কুরু { যদি (0) { (শূন্য)(assert_রাশিমালা); } } যাবত্(0)
#endif
YOSYS_NAMESPACE_END

#অন্তর্ভাব "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#অন্যথা
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
সংরচনা shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ধ্রুব বর্ণ *s) { content = std::shared_ptr<string>(new string(s)); }
	ধ্রুব বর্ণ *c_str() ধ্রুব { প্রত্যাগম content->c_str(); }
	ধ্রুব string &str() ধ্রুব { প্রত্যাগম *content; }
	bool operator==(ধ্রুব shared_str &other) ধ্রুব { প্রত্যাগম *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব {
		h.eat(*content);
		প্রত্যাগম h;
	}
};

namespace RTLIL {
	সংরচনা Const;
	সংরচনা SigBit;
	সংরচনা SigSpec;
	সংরচনা Wire;
	সংরচনা Cell;
	সংরচনা Memory;
	সংরচনা Process;
	সংরচনা Module;
	সংরচনা Design;
	সংরচনা Monitor;
	সংরচনা Selection;
	সংরচনা SigChunk;
	সংরচনা PooledName;
	গণনা State : unsigned বর্ণ;

	প্রকারনির্দেশ std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	সংরচনা Ast_গ্রন্থি;
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
	template<> সংরচনা hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<AST::Ast_গ্রন্থি*> : hash_obj_ops {};

	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Wire*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Cell*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Memory*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Process*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Module*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Design*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব RTLIL::Monitor*> : hash_obj_ops {};
	template<> সংরচনা hash_ops<ধ্রুব AST::Ast_গ্রন্থি*> : hash_obj_ops {};
}

শূন্য memhasher_on();
শূন্য memhasher_off();
শূন্য memhasher_do();

বাহ্য bool memhasher_active;
অন্তঃস্থ শূন্য memhasher() { যদি (memhasher_active) memhasher_do(); }

শূন্য yosys_banner();
পূর্ণাঙ্ক ceil_log2(পূর্ণাঙ্ক x) YS_ATTRIBUTE(ধ্রুব);

template<typename T> পূর্ণাঙ্ক GetSize(ধ্রুব T &obj) { প্রত্যাগম obj.size(); }
অন্তঃস্থ পূর্ণাঙ্ক GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
সংরচনা Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	স্থির bool active() { প্রত্যাগম active_; }
গুপ্ত:
	স্থির bool active_;
};

সংরচনা Autoidx {
	Autoidx(পূর্ণাঙ্ক মান) : মান(মান) {}
	operator পূর্ণাঙ্ক() ধ্রুব { প্রত্যাগম মান; }
	শূন্য ensure_at_least(পূর্ণাঙ্ক v);
	পূর্ণাঙ্ক operator++(পূর্ণাঙ্ক);
গুপ্ত:
	পূর্ণাঙ্ক মান;
};

বাহ্য Autoidx autoidx;
বাহ্য পূর্ণাঙ্ক yosys_xtrace;
বাহ্য bool yosys_write_versions;

ধ্রুব std::string *create_id_prefix(std::string_view file, পূর্ণাঙ্ক সারি, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ধ্রুব std::string * { \
		স্থির std::unique_ptr<ধ্রুব std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.সারি(), func)); \
		প্রত্যাগম prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ধ্রুব std::string * { \
		স্থির std::unique_ptr<ধ্রুব std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.সারি(), func)); \
		প্রত্যাগম prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
