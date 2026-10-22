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

#ضمن <array>
#ضمن <atomic>
#ضمن <map>
#ضمن <set>
#ضمن <tuple>
#ضمن <vector>
#ضمن <string>
#ضمن <algorithm>
#ضمن <functional>
#ضمن <unordered_map>
#ضمن <unordered_set>
#ضمن <initializer_list>
#ضمن <variant>
#ضمن <optional>
#ضمن <stdexcept>
#ضمن <memory>
#ضمن <cmath>
#ضمن <cstddef>

#ضمن <sstream>
#ضمن <fstream>
#ضمن <istream>
#ضمن <ostream>
#ضمن <iostream>

#ضمن <version>
#إذا __cpp_lib_source_location >= 201907L
	#ضمن <source_location>
	using std::source_location;
#وإلا
	#ضمن <experimental/source_location>
	using std::experimental::source_location;
#endif

#ضمن <stdarg.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <stdio.h>
#ضمن <limits.h>
#ضمن <sys/stat.h>
#ضمن <errno.h>

#ifndef _YOSYS_
#  error It looks like you are trying to build Yosys without the config defines set. \
         When building Yosys with a custom make system, make sure you set all the \
         defines the Yosys Makefile would set لكل your build configuration.
#endif

#ضمن "kernel/yosys_config.h"

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

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وإلاإذا defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#وإلا
#  define YS_ATTRIBUTE(...)
#endif

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_UNUSED __attribute__((__unused__))
#وإلا
#  define YS_MAYBE_UNUSED
#endif

#إذا defined(_MSC_VER)
#  define YS_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#وإلا
#  define YS_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#إذا __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#وإلا
#  error "C++20 or later compatible compiler is required"
#endif

#إذا defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#وإلا
#  define YS_COLD
#endif

#ifdef __cpp_consteval
#define YOSYS_CONSTEVAL consteval
#وإلا
// If we can't use consteval we can at least make it constexpr.
#define YOSYS_CONSTEVAL constexpr
#endif

#define YOSYS_ABORT(s) YOSYS_NAMESPACE_PREFIX log_yosys_abort_message(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] فراغ log_yosys_abort_message(std::string_view file, صحيح قۇر, std::string_view func, std::string_view message);

[[noreturn]]
فراغ log_assert_failure(ثابت حرف *ئىپادە, source_location location);
#ifndef NDEBUG
ساكن مضمن فراغ log_assert_worker(bool cond, ثابت حرف *ئىپادە, source_location location = source_location::current()) {
	إذا (!cond) log_assert_failure(ئىپادە, location);
}
#  define log_assert(assert_ئىپادە) YOSYS_NAMESPACE_PREFIX log_assert_worker(assert_ئىپادە, #_assert_expr_)
#وإلا
#  define log_assert(assert_ئىپادە) افعل { إذا (0) { (فراغ)(assert_ئىپادە); } } طالما(0)
#endif
YOSYS_NAMESPACE_END

#ضمن "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#ifdef HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#وإلا
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
بنية shared_str {
	std::shared_ptr<string> content;
	shared_str() { }
	shared_str(string s) { content = std::shared_ptr<string>(new string(s)); }
	shared_str(ثابت حرف *s) { content = std::shared_ptr<string>(new string(s)); }
	ثابت حرف *c_str() ثابت { أرجع content->c_str(); }
	ثابت string &str() ثابت { أرجع *content; }
	bool operator==(ثابت shared_str &other) ثابت { أرجع *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		أرجع h;
	}
};

namespace RTLIL {
	بنية Const;
	بنية SigBit;
	بنية SigSpec;
	بنية Wire;
	بنية Cell;
	بنية Memory;
	بنية Process;
	بنية Module;
	بنية Design;
	بنية Monitor;
	بنية Selection;
	بنية SigChunk;
	بنية PooledName;
	تعداد State : unsigned حرف;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	بنية Ast_تۈگۈن;
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
	template<> بنية hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	template<> بنية hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	template<> بنية hash_ops<RTLIL::Memory*> : hash_obj_ops {};
	template<> بنية hash_ops<RTLIL::Process*> : hash_obj_ops {};
	template<> بنية hash_ops<RTLIL::Module*> : hash_obj_ops {};
	template<> بنية hash_ops<RTLIL::Design*> : hash_obj_ops {};
	template<> بنية hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	template<> بنية hash_ops<AST::Ast_تۈگۈن*> : hash_obj_ops {};

	template<> بنية hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت RTLIL::Memory*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	template<> بنية hash_ops<ثابت AST::Ast_تۈگۈن*> : hash_obj_ops {};
}

فراغ memhasher_on();
فراغ memhasher_off();
فراغ memhasher_do();

خارجي bool memhasher_active;
مضمن فراغ memhasher() { إذا (memhasher_active) memhasher_do(); }

فراغ yosys_banner();
صحيح ceil_log2(صحيح x) YS_ATTRIBUTE(ثابت);

template<typename T> صحيح GetSize(ثابت T &obj) { أرجع obj.size(); }
مضمن صحيح GetSize(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
بنية Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ساكن bool active() { أرجع active_; }
خاص:
	ساكن bool active_;
};

بنية Autoidx {
	Autoidx(صحيح قىممەت) : قىممەت(قىممەت) {}
	operator صحيح() ثابت { أرجع قىممەت; }
	فراغ ensure_at_least(صحيح v);
	صحيح operator++(صحيح);
خاص:
	صحيح قىممەت;
};

خارجي Autoidx autoidx;
خارجي صحيح yosys_xtrace;
خارجي bool yosys_write_versions;

ثابت std::string *create_id_prefix(std::string_view file, صحيح قۇر, std::string_view func);

#define NEW_ID \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.قۇر(), func)); \
		أرجع prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}
#define NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_PREFIX TwineSpec{YOSYS_NAMESPACE_PREFIX TwineSpec::AutoSuffix{[](source_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> prefix(YOSYS_NAMESPACE_PREFIX create_id_prefix(location.file_name(), location.قۇر(), func)); \
		أرجع prefix.get(); \
	}(source_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::to_string(YOSYS_NAMESPACE_PREFIX autoidx++)}}

YOSYS_NAMESPACE_END

#endif
