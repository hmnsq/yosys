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

#شامل <array>
#شامل <atomic>
#شامل <map>
#شامل <set>
#شامل <tuple>
#شامل <vector>
#شامل <string>
#شامل <algorithm>
#شامل <functional>
#شامل <unordered_map>
#شامل <unordered_set>
#شامل <initializer_list>
#شامل <variant>
#شامل <optional>
#شامل <stdexcept>
#شامل <memory>
#شامل <cmath>
#شامل <cstddef>

#شامل <sstream>
#شامل <fstream>
#شامل <istream>
#شامل <ostream>
#شامل <iostream>

#شامل <version>
#اگر __cpp_lib_source_location >= 201907L
	#شامل <source_location>
	using std::source_location;
#وگرنه
	#شامل <experimental/source_location>
	using std::experimental::source_location;
#endif

#شامل <stdarg.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <stdint.h>
#شامل <stdio.h>
#شامل <limits.h>
#شامل <sys/stat.h>
#شامل <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set برای your build configuration.
#endif

#شامل "kernel/yosys_config.h"

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

#اگر defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وگرنهاگر defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#وگرنه
#  define YS_ATTRIBUTE(...)
#endif

#اگر defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#وگرنه
#  define YS_MAYBE_UNUSED
#endif

#اگر defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#وگرنه
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#اگر __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#وگرنه
#  error "C++20 or later compatible compiler is required"
#endif

#اگر defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#وگرنه
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#وگرنه
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] تهی log_yosys_abort_message(std::string_view file, صحیح سطر, std::string_view func, std::string_view message);

[[noreturn]]
تهی log_assert_failure(ثابت نویسه *عبارت, source_location location);
#ifndef NDEBUG
ایستا درونخطی تهی log_assert_worker(bool cond, ثابت نویسه *عبارت, source_location location = source_location::current()) {
	اگر (!cond) log_assert_failure(عبارت, location);
}
#  define log_assert(assert_عبارت) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_عبارت, #_assert_expr_)
#وگرنه
#  define log_assert(assert_عبارت) انجامبده { اگر (0) { (تهی)(assert_عبارت); } } وقتی(0)
#endif
YOSYS_NAMESPACE_END

#شامل "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#وگرنه
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
ساختار shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ثابت نویسه *s) { content = std::shared_ptr<string>(new string(s)); }
	ثابت نویسه *c_str() ثابت { بازگشت content->c_str(); }
	ثابت string &str() ثابت { بازگشت *content; }
	bool operator==(ثابت shared_str &other) ثابت { بازگشت *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		بازگشت h;
	}
};

namespace RTLIL {
	ساختار Const;
	ساختار SigBit;
	ساختار SigSpec;
	ساختار Wire;
	ساختار Cell;
	ساختار Memory;
	ساختار Process;
	ساختار Module;
	ساختار Design;
	ساختار Monitor;
	ساختار Selection;
	ساختار SigChunk;
	ساختار PooledName;
	شمارش State : unsigned نویسه;

	تعریفنوع std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	ساختار Ast_گره;
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
	template<> ساختار hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> ساختار hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> ساختار hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> ساختار hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> ساختار hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> ساختار hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> ساختار hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> ساختار hash_ops<AST::Ast_گره*> : hash_obj_ops {};

	template<> ساختار hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت RTLIL::Memory*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	template<> ساختار hash_ops<ثابت AST::Ast_گره*> : hash_obj_ops {};
}

تهی memhasher_on();
تهی memhasher_off();
تهی memhasher_do();

بیرونی bool memhasher_active;
درونخطی تهی memhasher() { اگر (memhasher_active) memhasher_do(); }

تهی yosys_banner();
صحیح ceil_log2(صحیح x) YS_ATTRIBUTE(ثابت);

template<typename T> صحیح GetSize(ثابت T &obj) { بازگشت obj.size(); }
درونخطی صحیح GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
ساختار Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ایستا bool active() { بازگشت active_; }
خصوصی:
	ایستا bool active_;
};

ساختار Autoidx {
	Autoidx(صحیح مقدار) : مقدار(مقدار) {}
	operator صحیح() ثابت { بازگشت مقدار; }
	تهی ensure_at_least(صحیح v);
	صحیح operator++(صحیح);
خصوصی:
	صحیح مقدار;
};

بیرونی Autoidx autoidx;
بیرونی صحیح yosys_xtrace;
بیرونی bool yosys_write_versions;

ثابت std::string *create_id_prefix(std::string_view file, صحیح سطر, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ثابت std::string * { \
		ایستا std::unique_ptr<ثابت std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.سطر(), func)); \
		بازگشت prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ثابت std::string * { \
		ایستا std::unique_ptr<ثابت std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.سطر(), func)); \
		بازگشت prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
