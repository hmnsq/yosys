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

#جيڪڏهن_وصف_نه_ٿيل YOSYS_COMMON_H
#وصف_ڪريو YOSYS_COMMON_H

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
	using std::ذريعو_location;
#وإلا
	#ضمن <experimental/source_location>
	using std::experimental::ذريعو_location;
#جيڪڏهن_پورو

#ضمن <stdarg.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <stdio.h>
#ضمن <limits.h>
#ضمن <sys/stat.h>
#ضمن <errno.h>

#جيڪڏهن_وصف_نه_ٿيل _YOSYS_
#  error It looks like you are trying منزل build Yosys without the config مقرر_ڪري سيٽ. \
         When building Yosys with a custom make system, make sure you سيٽ all the \
         مقرر_ڪري the Yosys Makefile would سيٽ لكل your build configuration.
#جيڪڏهن_پورو

#ضمن "kernel/yosys_config.h"

#وصف_ڪريو FRIEND_TEST(test_case_name, test_name) \
  دوست class test_case_name##_##test_name##_Test

#جيڪڏهن_وصف_ٿيل _WIN32
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
#    define رستو_MAX MAX_رستو
#    define isatty _isatty
#    define fileno _fileno
#  endif

// The following defines conflict with our identifiers:
#  undef CONST
// `wingdi.h` defines a TRANSPARENT macro that conflicts with X(TRANSPARENT) entry in kernel/constids.inc
#  undef TRANSPARENT
// `wingdi.h` defines an ERROR macro that conflicts with `ERROR()` macro in kernel/tclapi.cc
#  undef ERROR
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل رستو_MAX
#  define رستو_MAX 4096
#جيڪڏهن_پورو


#وصف_ڪريو YOSYS_NAMESPACE          Yosys
#وصف_ڪريو ذاتي_NAMESPACE_BEGIN  namespace {
#وصف_ڪريو ذاتي_NAMESPACE_پڇاڙي    }
#وصف_ڪريو YOSYS_NAMESPACE_BEGIN    namespace Yosys {
#وصف_ڪريو YOSYS_NAMESPACE_پڇاڙي      }
#وصف_ڪريو YOSYS_NAMESPACE_اڳياڙي   Yosys::
#وصف_ڪريو USING_YOSYS_NAMESPACE    using namespace Yosys;

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#وإلاإذا defined(_MSC_VER)
#  define YS_ATTRIBUTE(...)
#وإلا
#  define YS_ATTRIBUTE(...)
#جيڪڏهن_پورو

#إذا defined(__GNUC__) || defined(__clang__)
#  define YS_MAYBE_اڻ_استعمال_ٿيل __attribute__((__unused__))
#وإلا
#  define YS_MAYBE_اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

#إذا defined(_MSC_VER)
#  define YS_NO_UNIQUE_پتو [[msvc::no_unique_پتو]]
#وإلا
#  define YS_NO_UNIQUE_پتو [[no_unique_پتو]]
#جيڪڏهن_پورو

#إذا __cplusplus >= 202002L
#  define YS_FALLTHROUGH [[fallthrough]];
#وإلا
#  error "C++20 or later compatible compiler is required"
#جيڪڏهن_پورو

#إذا defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::cold)
#  define YS_COLD [[gnu::cold]]
#وإلا
#  define YS_COLD
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __cpp_consteval
#وصف_ڪريو YOSYS_CONSTEVAL consteval
#وإلا
// If we can't use consteval we can at least make it constexpr.
#وصف_ڪريو YOSYS_CONSTEVAL constexpr
#جيڪڏهن_پورو

#وصف_ڪريو YOSYS_ABORT(s) YOSYS_NAMESPACE_اڳياڙي log_yosys_abort_پيغام(__FILE__, __LINE__, __FUNCTION__, s)

// This has to precede including "kernel/io.h"
YOSYS_NAMESPACE_BEGIN
[[noreturn]] فراغ log_yosys_abort_پيغام(std::string_view file, صحيح سٽ, std::string_view func, std::string_view پيغام);

[[noreturn]]
فراغ log_assert_failure(ثابت حرف *اظهار, ذريعو_location location);
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
ساكن مضمن فراغ log_assert_ڪم_ڪندڙ(bool cond, ثابت حرف *اظهار, ذريعو_location location = ذريعو_location::current()) {
	إذا (!cond) log_assert_failure(اظهار, location);
}
#  define log_assert(assert_اظهار) YOSYS_NAMESPACE_اڳياڙي log_assert_ڪم_ڪندڙ(assert_اظهار, #_assert_expr_)
#وإلا
#  define log_assert(assert_اظهار) افعل { إذا (0) { (فراغ)(assert_اظهار); } } طالما(0)
#جيڪڏهن_پورو
YOSYS_NAMESPACE_پڇاڙي

#ضمن "kernel/io.h"

YOSYS_NAMESPACE_BEGIN

// Note: All headers included in hashlib.h must be included
// outside of YOSYS_NAMESPACE before this or bad things will happen.
#جيڪڏهن_وصف_ٿيل HASHLIB_H
#  error "You've probably included hashlib.h under two namespace paths. Bad idea."
#وإلا
#  include "kernel/hashlib.h"
#  undef HASHLIB_H
#جيڪڏهن_پورو


using std::ويڪٽر;
using std::string;
using std::tuple;
using std::pair;

using std::make_tuple;
using std::make_pair;
using std::get;
using std::min;
using std::max;

using hashlib::Hasher;
using hashlib::لاڳيتو_hash;
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
	bool عامل==(ثابت shared_str &other) ثابت { أرجع *content == *other.content; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(*content);
		أرجع h;
	}
};

namespace RTLIL {
	بنية Const;
	بنية Sig_ٻٽو_انگ;
	بنية SigSpec;
	بنية Wire;
	بنية Cell;
	بنية يادگيري;
	بنية Process;
	بنية Module;
	بنية Design;
	بنية Monitor;
	بنية Selection;
	بنية SigChunk;
	بنية PooledName;
	تعداد State : بنانشان حرف;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
}

namespace AST {
	بنية Ast_ڳنڍ;
}

using RTLIL::Const;
using RTLIL::Sig_ٻٽو_انگ;
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
	سانچو<> بنية hash_ops<RTLIL::Wire*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<RTLIL::Cell*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<RTLIL::يادگيري*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<RTLIL::Process*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<RTLIL::Module*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<RTLIL::Design*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<RTLIL::Monitor*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<AST::Ast_ڳنڍ*> : hash_obj_ops {};

	سانچو<> بنية hash_ops<ثابت RTLIL::Wire*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت RTLIL::Cell*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت RTLIL::يادگيري*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت RTLIL::Process*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت RTLIL::Module*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت RTLIL::Design*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت RTLIL::Monitor*> : hash_obj_ops {};
	سانچو<> بنية hash_ops<ثابت AST::Ast_ڳنڍ*> : hash_obj_ops {};
}

فراغ memhasher_on();
فراغ memhasher_off();
فراغ memhasher_do();

خارجي bool memhasher_سرگرم;
مضمن فراغ memhasher() { إذا (memhasher_سرگرم) memhasher_do(); }

فراغ yosys_banner();
صحيح ceil_log2(صحيح x) YS_ATTRIBUTE(ثابت);

سانچو<typename T> صحيح Get_سائيز(ثابت T &obj) { أرجع obj.سائيز(); }
مضمن صحيح Get_سائيز(RTLIL::Wire *wire);

// When multiple threads are accessing RTLIL, one of these guard objects
// must exist.
بنية Multithreading
{
	Multithreading();
	~Multithreading();
	// Returns true when multiple threads are accessing RTLIL.
	// autoidx cannot be used during such times.
	// IdStrings cannot be created during such times.
	ساكن bool سرگرم() { أرجع سرگرم_2; }
خاص:
	ساكن bool سرگرم_2;
};

بنية Autoidx {
	Autoidx(صحيح قدر) : قدر(قدر) {}
	عامل صحيح() ثابت { أرجع قدر; }
	فراغ ensure_at_گهٽ_کان_گهٽ(صحيح v);
	صحيح عامل++(صحيح);
خاص:
	صحيح قدر;
};

خارجي Autoidx autoidx;
خارجي صحيح yosys_xtrace;
خارجي bool yosys_write_versions;

ثابت std::string *create_id_اڳياڙي(std::string_view file, صحيح سٽ, std::string_view func);

#وصف_ڪريو NEW_ID \
	YOSYS_NAMESPACE_اڳياڙي TwineSpec{YOSYS_NAMESPACE_اڳياڙي TwineSpec::AutoSuffix{[](ذريعو_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> اڳياڙي_2(YOSYS_NAMESPACE_اڳياڙي create_id_اڳياڙي(location.file_name(), location.سٽ(), func)); \
		أرجع اڳياڙي_2.get(); \
	}(ذريعو_location::current(), __FUNCTION__), std::منزل_string(YOSYS_NAMESPACE_اڳياڙي autoidx++)}}
#وصف_ڪريو NEW_ID_SUFFIX(suffix) \
	YOSYS_NAMESPACE_اڳياڙي TwineSpec{YOSYS_NAMESPACE_اڳياڙي TwineSpec::AutoSuffix{[](ذريعو_location location, std::string_view func) -> ثابت std::string * { \
		ساكن std::unique_ptr<ثابت std::string> اڳياڙي_2(YOSYS_NAMESPACE_اڳياڙي create_id_اڳياڙي(location.file_name(), location.سٽ(), func)); \
		أرجع اڳياڙي_2.get(); \
	}(ذريعو_location::current(), __FUNCTION__), std::string(suffix) + "$" + std::منزل_string(YOSYS_NAMESPACE_اڳياڙي autoidx++)}}

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
